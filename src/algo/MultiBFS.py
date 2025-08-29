import geopandas as gpd
import pandas as pd

from src.obj.State import State
from src.obj.District import District
from src.obj.Precinct import Precinct

import random
from collections import deque

class MultiBFS:

    def __init__(self):
        pass

    def draw(self, state: State, gdf: gpd.GeoDataFrame):
        pctLoc = -1
        dists = state.dists
        assigned = state.assigned

        pctAssns = [-1] * len(gdf)
        queues = []
        for dist in dists:
            queues.append(deque())
        qdTuple = list(zip(queues, dists))

        print("Using District Nuclei:")
        for tuple in qdTuple:
            queue, dist = tuple
            while True:
                pctLoc = int(random.random() * state.numPrecincts)
                if pctAssns[pctLoc] == -1:
                    break
            pct = state.getPrecinct(pctLoc)
            print(f"\t{pct.name}")
            queue.append(pct)

        i = 0
        while True:
            if (not any(queues)) or all(dist.isFull() for dist in dists): 
                # All districts are done adding precincts
                break 

            dist = state.dists[i]
            if (not queues[i]) or dist.isFull(): 
                # This district is done adding precincts
                queues[i].clear()
                i += 1
                i = i % state.numDists
                continue
            
            curr = queues[i].popleft()
            while curr in assigned and queues[i]:
                curr = queues[i].popleft()
            if curr not in assigned and (dist.borders(curr) or not dist.precincts):
                state.assign(curr, i)
                pctAssns[curr.index] = i + 1

            addNeighborsToQueue(assigned, queues[i], dist, curr)

            # Cycle through districts
            i += 1
            i = i % state.numDists

        # Add leftover unassigned precincts to districts
        unassigned = state.unassigned
        print(f"Initial assignment completed. Adding {len(unassigned)} remaining precincts to districts...")
        unassignedL = list(unassigned)
        i = 0
        while unassignedL:
            curr = unassignedL[i]
            for dist in state.dists:
                if dist.borders(curr):
                    state.assign(curr, dist.id-1)
                    pctAssns[curr.index] = dist.id
                    unassignedL.remove(curr)
                    break
            i += 1
            if i >= len(unassignedL):
                i = 0


        gdf['barddist'] = pctAssns
        return gdf

def addNeighborsToQueue(assigned: set, queue: deque, dist: District, pct: Precinct):
    neighbors = pct.neighbors
    for neighbor in neighbors:
        if neighbor not in assigned and dist.borders(neighbor) and neighbor not in queue:
            queue.append(neighbor)
