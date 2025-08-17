import geopandas as gpd

from collections import deque

def draw(totPop: int, num: int, gdf: gpd.GeoDataFrame):
    queue = deque()
    visited = set()

    dist = []
    target = []
    pop = totPop
    district = 1

    for i in range(0, num):
        target.append(totPop // num)
        pop = pop - target[i]

    i = 0
    while pop > 0:
        target[i] += 1
        pop -= 1
        i += 1

    queue.append(gdf.loc[0])

    while(queue):
        curr = queue.popleft()
        if curr.get('index') not in visited:
            visited.add(curr.get('index'))

            neighbors = curr.get('neighbors')

            for j in neighbors:
                queue.append(gdf.loc[j])

            


