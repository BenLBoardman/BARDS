import geopandas as gpd

from src.obj.District import District

import random
import time
from collections import deque
class SimpleBFS:

    def __init__(self):
        pass
    
    # Draw a map. This function header is recommended but not required. Returning a modified dataframe is also strongly recommended.
    def draw(self, totPop: int, numDists: int, gdf: gpd.GeoDataFrame):
        queue = deque()
        visited = set()

        distList = [-1] * len(gdf)
        dists = []
        pop = totPop
        distNum = 1
    
        startTime = time.time()
        for i in range(0, numDists):
            dists.append(District(i, totPop // numDists))
            pop = pop - dists[i].tgt

        i = 0
        while pop > 0:
            dists[i].tgt += 1
            pop -= 1
            i += 1

        startingLoc = int(random.random() * len(gdf))
        print(f"Starting from precinct {gdf.loc[startingLoc].get('name')}...")
        queue.append(gdf.loc[startingLoc])

        currDist = dists[0]
        while(queue):
            curr = queue.popleft()
            if curr.get('index') not in visited:
                visited.add(curr.get('index'))

                neighbors = curr.get('neighbors')

                for j in neighbors:
                    queue.append(gdf.loc[j])
            
                distList[curr.get('index')] = distNum

                currDist.addPrecinct(curr)
                if distNum < numDists and currDist.isFull():
                        currDist = dists[distNum]
                        distNum += 1
    
        print(f"Districts computed in {round(time.time() - startTime, 3)} seconds...")

        for i in range(0, distNum):
            dist = dists[i]
            if not dist.isContiguous(gdf):
                print(f"WARNING: District {i + 1} is not contiguous")
        gdf['barddist'] = distList
        return gdf
      

    

