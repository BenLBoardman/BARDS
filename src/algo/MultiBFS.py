import geopandas as gpd

from src.obj.District import District

import random
from collections import deque

class MultiBFS:

    def __init__(self):
        pass

    def draw(self, totPop: int, dists: list[District], numDists: int, gdf: gpd.GeoDataFrame):
        assigned = set()
        pctLoc = -1

        currDist = dists[0]

        pctAssns = [-1] * len(gdf)

        queues = [deque()] * numDists
        qdTuple = list(zip(queues, dists))

        for tuple in qdTuple:
            queue, dist = tuple
            while True:
                pctLoc = int(random.random() * len(gdf))
                if pctLoc not in assigned:
                    break
            pct = gdf.loc[pctLoc]
            queue.append(pct)
            assigned.add(pctLoc)
            dist.addPrecinct(pct)
            pctAssns[pctLoc] = dist.id

        gdf['barddist'] = pctAssns
        return (gdf, dists)