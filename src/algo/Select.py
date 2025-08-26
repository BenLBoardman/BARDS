import geopandas as gpd

from src.algo import SimpleBFS as sbfs
from src.algo import MultiBFS as mbfs

# Select an algorithm from the list of valid algorithms
def selectAlgo(algo: str, population: int, numDists: int, gdf: gpd.GeoDataFrame):
    drawer = None
    # Insert new algorithms into the if-else HERE.
    if algo == "simple":
        drawer =  sbfs.SimpleBFS()
    if algo == "multi":
        drawer =  mbfs.MultiBFS()
    else:
        print(f"Algorithm {algo} not recognized, districts not drawn.")
        
    
    return drawer