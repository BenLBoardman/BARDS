from src.obj.District import District
from src.obj.Precinct import Precinct

import pandas as pd

# STATE OBJECT CURRENTLY UNUSED
class State:
    def __init__(self, id: str, pop: int, distCt: int, df: pd.DataFrame):
        self.id = id
        self.pop = pop
        self.mkDistObjs(distCt)
        self.numDists = distCt
        self.mkPrecincts(df)
        self.numPrecincts = len(self.precincts)
        self.unassigned = set(self.precincts.copy())
        self.assigned = set()
        self.neighborIndexToPrecinct()
        self.deviation = 0
        self.avgTgt = int(pop / distCt)
        self.smallestDist = None
        self.largestDist = None
        self.maxDev = 0.0075 # Max acceptable deviation
    
    def assign(self, precinct: Precinct, district: District, updateDev: bool=False):
        self.assigned.add(precinct)
        self.unassigned.remove(precinct)
        if district not in self.dists:
            print(f"District {district.id} is not in the State!")
            return False
        district.addPrecinct(precinct, self.dists)
        if updateDev:
            self.updateDeviation()
        return True

    def unassign(self, precinct: Precinct, district: District, updateDev: bool=False):
        self.assigned.remove(precinct)
        self.unassigned.add(precinct)
        if district not in self.dists:
            print(f"District {district.id} is not in the State!")
            return False
        district.removePrecinct(precinct, self.dists)
        if updateDev:
            self.updateDeviation()
        return True

    def swap(self, prec1: Precinct, prec2: Precinct, updateDev: bool=False):
        dist1 = prec1.district
        dist2 = prec2.district
        self.unassign(prec1, dist1)
        self.unassign(prec2, dist2)
        self.assign(prec2, dist1)
        self.assign(prec1, dist2)
        if updateDev:
            self.updateDeviation()

    def updateDeviation(self):
        for dist in self.dists:
            if self.smallestDist == None or dist.pop < self.smallestDist.pop:
                self.smallestDist = dist
        
            if self.largestDist == None or dist.pop > self.largestDist.pop:
                self.largestDist = dist
        
        self.deviation = round((self.largestDist.pop - self.smallestDist.pop)/self.avgTgt, 4)

    def mkDistObjs(self, distCt: int):
        self.dists = []
        pop = self.pop
        for i in range(0, distCt):
            self.dists.append(District(i+1, self.pop // distCt))
            pop = pop - self.dists[i].tgt

        i = 0
        while pop > 0:
            self.dists[i].tgt += 1
            pop -= 1
            i += 1
    
    def mkPrecincts(self, df: pd.DataFrame):
        self.precincts = []
        for (i, row) in df.iterrows():
            precinct = Precinct(row.copy(True))
            self.precincts.append(precinct)
        return self.precincts
    
    def neighborIndexToPrecinct(self):
        for precinct in self.precincts:
            neighbors = []
            for neighbor in precinct.neighbors:
                obj = next((obj for obj in self.precincts if obj.index == neighbor), None)
                neighbors.append(obj)
            precinct.neighbors = neighbors

    # Successive Swap Rebalance - rebalance district pops by successively swapping neighbor precincts between districts
    # SS rebalance is not enough - districts under a certain size need to also just take precincts from their most populous neighbor
    def ssRebalance(self):
        swaps = 0
        currDev = 0
        lastSwap = None
        done = False
        while self.deviation > self.maxDev / 2:
            if swaps % 2 == 0: #alternate between the smallest district taking a precinct from its largest neighbor
                self.smallestDist.takePrecinctFrom(self.smallestDist.getLargestNeighbor(), self.dists)
            else: # and the largest district giving a precinct to its smallest neighbor 
                self.largestDist.givePrecinctTo(self.largestDist.getSmallestNeighbor(), self.dists)
            swaps += 1
            self.updateDeviation()
            if swaps % 100 == 0:
                print(f"Deviation after {swaps} swaps: {round(self.deviation*100,2)}%")
        while False and self.deviation > self.maxDev and not done:
            for precinct in self.precincts:
                if lastSwap == precinct:
                    print("No more immediately useful swaps can be made, exiting.")
                    done = True
                    break
                for neighbor in precinct.neighbors:
                    currDev = self.deviation
                    if not precinct.district == neighbor.district:
                        self.swap(precinct, neighbor, True)
                        if currDev <= self.deviation or not precinct.district.isContiguous() or not neighbor.district.isContiguous():
                            self.swap(precinct, neighbor, True)
                        else:
                            swaps += 1
                            lastSwap = precinct
                            if swaps % 10 == 0:
                                print(f"Deviation after {swaps} swaps: {round(self.deviation*100,2)}%")
                            break
        print(f"Successive-swap rebalancing complete in {swaps} swaps. Final deviation is {round(self.deviation*100, 2)}%...")


    def getPrecinct(self, index: int):
        return next((obj for obj in self.precincts if obj.index == index), None)

    def doWarnings(self):
        for i in range(0, len(self.dists)):
            dist = self.dists[i]
            if dist.isTooBig():
                print(f"WARNING: District {i+1} is too large (District size {round((dist.pop * 100) / dist.tgt, 2)}% of target)")
            if dist.isTooSmall():
                print(f"WARNING: District {i+1} is too small (District size {round((dist.pop * 100) / dist.tgt, 2)}% of target)")
            if not dist.isContiguous():
                print(f"WARNING: District {i+1} is not contiguous") 
        if self.unassigned:
            print(f"WARNING: {len(self.unassigned)} of {len(self.precincts)} precincts are not assigned to districts")
        if self.deviation > .0075:
            print(f"WARNING: Statewide population deviation is {round(self.deviation*100, 2)}%, it should be less than 0.75%.")
    
    def clear(self):
        self.assigned = set()
        self.unassigned = set(self.precincts.copy())
        for dist in self.dists:
            dist.clear()