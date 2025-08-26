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
        queues = []
        for dist in dists:
            queues.append(deque())
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
            queue.append(pctLoc)
            pctAssns[pctLoc] = dist.id

        i = 0
        while True:
            if (not any(queues)) or all(dist.isFull() for dist in dists): 
                # All districts are done adding precincts
                break 
            if (not queues[i]) or dists[i].isFull(): 
                # This district is done adding precincts
                queues[i].clear()
                i += 1
                i = i % numDists
                continue
            
            index = queues[i].popleft()
            while index in assigned and queues[i]:
                index = queues[i].popleft()
            curr = gdf.loc[index]
            pct = Precinct(curr)
            if index not in assigned and (dists[i].borders(pct) or not dists[i].precincts):
                assigned.add(index)
                dists[i].addPrecinct(pct)
                pctAssns[index] = i + 1

            addNeighborsToQueue(assigned, queues[i], dists[i], pct, gdf)

            # Cycle through districts
            i += 1
            i = i % numDists

        gdf['barddist'] = pctAssns
        return (gdf, dists)

def addNeighborsToQueue(assigned: set, queue: deque, dist: District, pct: Precinct, gdf: gpd.geodataframe):
    neighbors = pct.neighbors
    for j in neighbors:
        neighbor = gdf.loc[j]
        if j not in assigned and dist.bordersPD(neighbor) and j not in queue:
            queue.append(j)