import geopandas as gpd
import pandas as pd

from src.obj.Precinct import Precinct

from collections import deque

class District:
    def __init__(self, id: int, tgtPop: int):
        self.id = id
        self.tgt = tgtPop
        self.pop = 0
        self.precincts = []
        self.maxdev = 0.0075 # maximum per-district population deviation that will be achieved before precincts will stop being added - set to 0.75% by default

    def addPrecinctPD(self, precinct: pd.Series):
        self.precincts.append(Precinct(precinct))
        self.pop += precinct['TOTPOP']
        
    def addPrecinct(self, precinct: Precinct):
        self.precincts.append(precinct)
        precinct.addToDistrict(self)
        self.pop += precinct.pop

    def getPrecinctFromIndex(self, index: int):
        for precinct in self.precincts:
            if precinct.index == index:
                return precinct
        return None

    def isFull(self):
        return self.pop >= (self.tgt * (1- self.maxdev / 5))
    
    def isTooSmall(self):
        return self.pop < self.tgt * (1- self.maxdev)
    
    def isTooBig(self):
        return self.pop > self.tgt * (1 + self.maxdev)
    
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

    def toDataFrame(self, gdf: gpd.GeoDataFrame):
        return gpd.GeoDataFrame(gdf[gdf['index'].isin(self.precincts)])
