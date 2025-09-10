import geopandas as gpd
import pandas as pd

from src.obj.State import State
from src.obj.District import District
from src.obj.Precinct import Precinct

import random
from collections import deque

class MultiBFS:

    def __init__(self):
        self.retries = 0

    def draw(self, state: State, gdf: gpd.GeoDataFrame):

        pctAssns = [-1] * len(gdf)

        self.makeNuclei(state, pctAssns)
        self.buildMap(state, pctAssns)
        while state.deviation > 0.04 * state.numDists: # note - this method works well for all states tested so far (NH, NV, TN, NJ) except Oregon
            print(f"State deviation is {round(state.deviation * 100, 2)}%, regenerating map.")
            self.retries += 1
            pctAssns = [-1] * len(state.precincts)
            self.makeNucleus(state.smallestDist, state.largestDist.precincts, pctAssns)
            self.makeNucleus(state.largestDist, state.largestDist.precincts, pctAssns)

            for dist in state.dists:
                pct = dist.nucleus
                pctAssns[pct.index] = dist.id+1
            state.clear()
            self.buildMap(state, pctAssns)

        print(f"Generated base map with {self.retries} retries...")
        if state.deviation >= state.maxDev:
            print(f"Beginning successive-swap rebalancing with {round(state.deviation*100,2)}% deviation...")
            state.ssRebalance()

        gdf['barddist'] = [precinct.district.id for precinct in state.precincts]
        return gdf

    def makeNuclei(self, state: State, pctAssns: list):
        if self.retries == 0:
            print("Using District Nuclei:")
        for dist in state.dists:
            self.makeNucleus(dist, state.precincts, pctAssns)

    def makeNucleus(self, dist: District, pcts: set, pctAssns: list):
        while True:
            pct = random.sample(pcts, 1)[0]
            if pctAssns[pct.index] == -1:
                break
        dist.nucleus = pct
        if self.retries == 0:
            print(f"\t{pct.name}")

    def buildMap(self, state: State, pctAssns: list):
        dists = state.dists
        assigned = state.assigned
        queues = []
        for dist in dists:
            queue = deque()
            queue.append(dist.nucleus)
            queues.append(queue)


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
                state.assign(curr, dist)
                pctAssns[curr.index] = i + 1

            addNeighborsToQueue(assigned, queues[i], dist, curr)

            # Cycle through districts
            i += 1
            i = i % state.numDists

        # Add leftover unassigned precincts to districts
        unassigned = state.unassigned
        if self.retries == 0:
            print(f"Initial assignment completed. Adding {len(unassigned)} remaining precincts to districts...")
        unassignedL = list(unassigned)
        i = 0
        while unassignedL:
            curr = unassignedL[i]
            for dist in state.dists:
                if dist.borders(curr):
                    state.assign(curr, dist)
                    pctAssns[curr.index] = dist.id
                    unassignedL.remove(curr)
                    break
            i += 1
            if i >= len(unassignedL):
                i = 0


        state.updateDeviation()

def addNeighborsToQueue(assigned: set, queue: deque, dist: District, pct: Precinct):
    neighbors = pct.neighbors
    for neighbor in neighbors:
        if neighbor not in assigned and dist.borders(neighbor) and neighbor not in queue:
            queue.append(neighbor)
