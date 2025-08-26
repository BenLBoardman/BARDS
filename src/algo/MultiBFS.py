import geopandas as gpd
import pandas as pd

from src.obj.District import District
from src.obj.Precinct import Precinct

import random
from collections import deque

class MultiBFS:

    def __init__(self):
        pass

    def draw(self, totPop: int, dists: list[District], numDists: int, gdf: gpd.GeoDataFrame):
        assigned = set()
        pctLoc = -1


        pctAssns = [-1] * len(gdf)

        queues = [deque()] * numDists
        qdTuple = list(zip(queues, dists))

        print("Using District Nuclei:")
        for tuple in qdTuple:
            queue, dist = tuple
            while True:
                pctLoc = int(random.random() * len(gdf))
                if pctAssns[pctLoc] == -1:
                    break
            pct = gdf.loc[pctLoc]
            print(f"\t{pct.get('name')}")
            queue.append(pct)
            dist.addPrecinctPD(pct)
            pctAssns[pctLoc] = dist.id

        i = 0
        while True:
            if (not any(queues)) or all(dist.isFull() for dist in dists): 
                # All districts are done adding precincts
                break 
            if (not queues[i]) or dists[i].isFull(): 
                # This district is done adding precincts
                i += 1
                i = i % numDists
                continue
            
            curr = queues[i].popleft()
            pct = Precinct(curr)
            index = curr.get('index')
            if index not in assigned:

                if not dists[i].isFull():
                    assigned.add(index)
                    dists[i].addPrecinct(pct)
                    pctAssns[index] = i + 1

                    addNeighborsToQueue(queue, dists[i], pct, gdf)

            # Cycle through districts
            i += 1
            i = i % numDists

        gdf['barddist'] = pctAssns
        return (gdf, dists)

def addNeighborsToQueue(queue: deque, dist: District, pct: Precinct, gdf: gpd.geodataframe):
    neighbors = pct.neighbors
    for j in neighbors:
        neighbor = gdf.loc[j]
        if dist.bordersPD(neighbor):
            queue.append(neighbor)