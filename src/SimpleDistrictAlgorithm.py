import geopandas as gpd

import random
from collections import deque

def draw(totPop: int, numDists: int, gdf: gpd.GeoDataFrame):
    queue = deque()
    visited = set()

    distList = [-1] * len(gdf)
    target = []
    pop = totPop
    distNum = 1
    

    for i in range(0, numDists):
        target.append(totPop // numDists)
        pop = pop - target[i]

    i = 0
    while pop > 0:
        target[i] += 1
        pop -= 1
        i += 1

    startingLoc = int(random.random() * len(gdf))
    print(f"Starting from precinct no. {startingLoc} of {len(gdf)}")
    queue.append(gdf.loc[0])

    while(queue):
        curr = queue.popleft()
        if curr.get('index') not in visited:
            visited.add(curr.get('index'))

            neighbors = curr.get('neighbors')

            for j in neighbors:
                queue.append(gdf.loc[j])
            
            distList[curr.get('index')] = distNum
            pop += int(curr.get("TOTPOP"))
            if distNum < numDists and pop >= (target[distNum] - 1000):
                    distNum += 1
                    pop = 0
    
    gdf['barddist'] = distList
    return gdf
      
      

    

