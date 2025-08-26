import geopandas as gpd

from src.obj.District import District

import random
import time
from collections import deque
class SimpleBFS:

    def __init__(self):
        pass
    
    # Draw a map. This function header is recommended but not required. Returning a modified dataframe is also strongly recommended.
    def draw(self, totPop: int, dists: list[District], numDists: int, gdf: gpd.GeoDataFrame):
        queue = deque()
        visited = set()

        distList = [-1] * len(gdf)
        distNum = 1
    
        startTime = time.time()

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

        gdf['barddist'] = distList
        return (gdf, dists)
      

    

