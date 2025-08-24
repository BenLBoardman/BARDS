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

    def addPrecinct(self, precinct: pd.Series):
        self.precincts.append(Precinct(precinct))
        self.pop += precinct['TOTPOP']

    def getPrecinctFromIndex(self, index: int):
        for precinct in self.precincts:
            if precinct.index == index:
                return precinct
        return None

    def isFull(self):
        return self.pop >= self.tgt
    
    def isTooSmall(self):
        return self.pop < self.tgt * (1- self.maxdev)
    
    def isTooBig(self):
        return self.pop > self.tgt * (1 + self.maxdev)
    
    def isContiguous(self):
        queue = deque()
        visited = set()
        all = set(self.precincts)

        queue.append(self.precincts[0])

        while(queue):
            curr = queue.popleft()
            if curr not in visited:
                visited.add(curr)

                neighbors = curr.neighbors

                for j in neighbors:
                    potential = self.getPrecinctFromIndex(j)
                    if potential != None:
                        queue.append(potential)

        return visited == all
    

    def toDataFrame(self, gdf: gpd.GeoDataFrame):
        return gpd.GeoDataFrame(gdf[gdf['index'].isin(self.precincts)])
