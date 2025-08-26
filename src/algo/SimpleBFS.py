import geopandas as gpd

from src.obj.District import District

import random
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

                currDist.addPrecinctPD(curr)
                if distNum < numDists and currDist.isFull():
                        currDist = dists[distNum]
                        distNum += 1

        gdf['barddist'] = distList
        return (gdf, dists)
      

    

