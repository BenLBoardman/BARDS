import geopandas as gpd

from src.obj.State import State
from src.obj.District import District
from src.obj.District import Precinct


import random
from collections import deque
class SimpleBFS:

    def __init__(self):
        pass
    
    # Draw a map. This function header is recommended but not required. Returning a modified dataframe is also strongly recommended.
    def draw(self, state: State, gdf: gpd.GeoDataFrame):
        queue = deque()

        distList = [-1] * len(gdf)
        distNum = 1
    
        startingLoc = int(random.random() * len(gdf))
        print(f"Starting from precinct {gdf.loc[startingLoc].get('name')}...")
        queue.append(state.getPrecinct(startingLoc))

        currDist = state.dists[0]
        visited = state.assigned
        while(queue):
            curr = queue.popleft()
            if curr not in visited:
                visited.add(curr)

                neighbors = curr.neighbors

                for neighbor in neighbors:
                    queue.append(neighbor)
            
                distList[curr.index] = distNum

                currDist.addPrecinct(curr)
                if distNum < state.numDists and currDist.isFull():
                        currDist = state.dists[distNum]
                        distNum += 1

        gdf['barddist'] = distList
        return gdf
      

    

