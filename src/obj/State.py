from src.obj.District import District
from src.obj.Precinct import Precinct

import pandas as pd
import time as time
import random

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
        return self.deviation

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
        act = True
        start = time.time()
        smd = True
        lgd = True
        i = 0
        xferHist = dict()
        while False and self.deviation > self.maxDev / 2:
            self.dists.sort(key=lambda dist:dist.pop)

            curr = self.dists[i]
            next = curr.getLargestNeighbor()
            prev = curr
            i = (i + 1) % self.numDists
            while next.pop >= curr.pop:
                if self.transferPrecinct(next, curr, xferHist):
                    swaps += 1
                prev = curr
                curr = next
                next = curr.getLargestNeighbor()
                if swaps % 10 == 0:
                    print(f"Deviation after {swaps} swaps: {round(self.deviation*100,2)}%")
        while self.deviation > self.maxDev / 2 and act:
            self.dists.sort(key=(lambda dist: dist.pop))
            if swaps % 2 == 0: #alternate between the smallest district taking a precinct from its largest neighbor
                i = 0
                smd = False
                while not smd and i < round(self.numDists / 2 - 1): 
                    smd = self.transferPrecinct(self.dists[i].getLargestNeighbor(), self.dists[i], xferHist)
                    i += 1
                    
            else: # and the largest precinct giving a precinct to its smallest neighbor
                lgd = False
                i = self.numDists - 1
                while not lgd and i > round(self.numDists / 2 - 1):
                    lgd = self.transferPrecinct(self.dists[i], self.dists[i].getSmallestNeighbor(), xferHist)
                    i -= 1
            act = smd or lgd
            swaps += 1
            self.updateDeviation()
            if swaps % 100 == 0 or (swaps > 1000 and swaps % 10 == 0):
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
        print(f"Rebalancing averaged {round((time.time() - start)/swaps,4)} seconds per swap.")

    def transferPrecinct(self, source: District, dest: District, hist: dict):
        hash = (lambda d1, d2, p: (source.id, dest.id, precinct.index))
        startingDev = self.deviation
        otp = False
        if dest not in source.neighbors:
            print(f"Attempted to transfer precinct from district {source.id} to non-neighbor {dest.id}")
            return False
        valid = [p for p in source.precincts if any([n for n in p.neighbors if n.district == dest])]
        random.shuffle(valid)
        for precinct in valid: # random sort prevents a loop of the same set of transfers
            self.unassign(precinct, source)
            precincts = [precinct]
            random.shuffle(precinct.neighbors)
            for neighbor in precinct.neighbors:
                # if any neighbors of the chosen precinct only border the chosen precinct, remove them too
                if len(neighbor.neighbors) == 1 and neighbor.district == source:
                    self.unassign(neighbor, source)
                    precincts.append(neighbor)
            histEntry = (source,  dest, tuple(precincts))
            
            if source.isContiguous() and not hist.get(hash(source, dest, precinct)) == histEntry:
                [self.assign(p, dest) for p in precincts]
                hist[hash(source, dest, precinct)] = histEntry
                otp = True
                break
            else:
                [self.assign(p, source) for p in precincts]
        self.updateDeviation()
        # print(f"No contiguous transfers can be made from district {self.id} to district {other.id}")
        return otp

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