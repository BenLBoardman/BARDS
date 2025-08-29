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
        self.unassigned = self.precincts.copy()
        self.assigned = set()
        self.neighborIndexToPrecinct()
        self.deviation = 0
        self.smallestDist = None
        self.largestDist = None
    
    def assign(self, precinct: Precinct, i: int):
        self.assigned.add(precinct)
        self.unassigned.remove(precinct)
        self.dists[i].addPrecinct(precinct, self.dists)

    def unassign(self, precinct: Precinct, i: int):
        self.assigned.remove(precinct)
        self.unassigned.add(precinct)
        self.dists[i].removePrecinct(precinct, self.dists)

    def swap(self, prec1: Precinct, prec2: Precinct):
        dist1 = prec1.district.id
        dist2 = prec2.district.id
        self.unassign(prec1, dist1)
        self.unassign(prec2, dist2)
        self.assign(prec2, dist1)
        self.assign(prec1, dist2)

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
        self.precincts = set()
        for (i, row) in df.iterrows():
            precinct = Precinct(row)
            self.precincts.add(precinct)
        return self.precincts
    
    def neighborIndexToPrecinct(self):
        for precinct in self.precincts:
            neighbors = []
            for neighbor in precinct.neighbors:
                obj = next((obj for obj in self.precincts if obj.index == neighbor), None)
                neighbors.append(obj)
            precinct.neighbors = neighbors

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
            
    def updateSmallestDistrict(self):
        pass

    def updateLargestDistrict(self):
        pass