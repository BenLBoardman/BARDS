import geopandas as gpd
import pandas as pd

from src.obj.Precinct import Precinct

from collections import deque
import random

class District:
    def __init__(self, id: int, tgtPop: int):
        self.id = id
        self.tgt = tgtPop
        self.pop = 0
        self.precincts = []
        self.deviation = 0
        self.maxdev = 0.0075 # maximum per-district population deviation that will be achieved before precincts will stop being added - set to 0.75% by default
        self.neighbors = set()
        self.nucleus = None

    def addPrecinctPD(self, precinct: pd.Series):
        self.precincts.append(Precinct(precinct))
        self.pop += precinct['TOTPOP']
        self.deviation = abs(1 - round(self.pop / self.tgt, 4))
        
    def addPrecinct(self, precinct: Precinct, dists):
        self.precincts.append(precinct)
        precinct.addToDistrict(self)
        self.pop += precinct.pop
        self.deviation = abs(1 - round(self.pop / self.tgt, 4))
        self.updateNeighbors(precinct, dists)

    def removePrecinct(self, precinct: Precinct, dists):
        self.precincts.remove(precinct)
        precinct.removeFromDistrict()
        self.pop -= precinct.pop
        self.deviation = abs(1 - round(self.pop / self.tgt, 4))
        self.updateNeighbors(precinct, dists, True)

    def givePrecinctTo(self, other, dists):
        if other not in self.neighbors:
            print(f"Attempted to transfer precinct from district {self.id} to non-neighbor {other.id}")
            return False
        valid = [p for p in self.precincts if any([n for n in p.neighbors if n.district == other])]
        for precinct in random.sample(valid, len(valid)): # random sort prevents a loop of the same set of transfers
            self.removePrecinct(precinct, dists)
            precincts = [precinct]
            for neighbor in random.sample(precinct.neighbors, len(precinct.neighbors)):
                # if any neighbors of the chosen precinct only border the chosen precinct, remove them too
                if len(neighbor.neighbors) == 1 and neighbor.district == self:
                    self.removePrecinct(neighbor, dists)
                    precincts.append(neighbor)
            if self.isContiguous():
                [other.addPrecinct(p, dists) for p in precincts]
                return True
            else:
                [self.addPrecinct(p, dists) for p in precincts]
        # print(f"No contiguous transfers can be made from district {self.id} to district {other.id}")
        return False

    def getLargestNeighbor(self):
        largestNeighbor = None
        largestNeighborPop = -1
        for neighbor in self.neighbors:
            if neighbor.pop > largestNeighborPop:
                largestNeighbor = neighbor
                largestNeighborPop = neighbor.pop
        return largestNeighbor
        
    def getSmallestNeighbor(self):
        smallestNeighbor = None
        smallestNeighborPop = 2000000000
        for neighbor in self.neighbors:
            if neighbor.pop < smallestNeighborPop:
                smallestNeighbor = neighbor
                smallestNeighborPop = neighbor.pop
        return smallestNeighbor
    
    def updateNeighbors(self, precinct: Precinct, dists, remove=False):
        if remove:
            for dist in list(self.neighbors):
                if not any(dist.borders(precinct) for precinct in self.precincts):
                    self.neighbors.remove(dist)
                    dist.neighbors.remove(self)
            return True
        for dist in dists:
            if (not dist == self) and dist.borders(precinct):
                dist.addNeighbor(self)
                self.addNeighbor(dist)
                    

    def addNeighbor(self, other):
        self.neighbors.add(other)

    def getPrecinctFromIndex(self, index: int):
        for precinct in self.precincts:
            if precinct.index == index:
                return precinct
        return None

    def isFull(self):
        return self.pop >= (self.tgt * (1- self.maxdev / 5))
    
    def isTooSmall(self):
        return self.pop < self.tgt and self.deviation > self.maxdev
    
    def isTooBig(self):
        return self.pop > self.tgt and self.deviation > self.maxdev
    
    def isContiguous(self):
        queue = deque()
        visited = set()
        all = set(self.precincts)
        if self.precincts:
            queue.append(self.precincts[0])

        while(queue):
            curr = queue.popleft()
            if curr not in visited:
                visited.add(curr)

                neighbors = curr.neighbors

                for neighbor in neighbors:
                    if neighbor in self.precincts:
                        queue.append(neighbor)

        return visited == all
    

    def borders(self, precinct: Precinct):
        for neighbor in precinct.neighbors:
            if neighbor in self.precincts:
                return True
        return False
    

    def bordersPD(self, pct: pd.Series):
        indices = [pct.index for pct in self.precincts]
        for neighbor in pct.get('neighbors'):
            if neighbor in indices:
                return True
        return False
    
    def makeDistrictObjects(totPop: int, distCt: int):
        dists = []
        pop = totPop
        for i in range(0, distCt):
            dists.append(District(i+1, totPop // distCt))
            pop = pop - dists[i].tgt

        i = 0
        while pop > 0:
            dists[i].tgt += 1
            pop -= 1
            i += 1
        
        return dists
    
    def clear(self):
        self.pop = 0
        for precinct in self.precincts:
            precinct.district = None
        self.precincts = []
        self.neighbors.clear()

    def toDataFrame(self, gdf: gpd.GeoDataFrame):
        return gpd.GeoDataFrame(gdf[gdf['index'].isin(self.precincts)])
