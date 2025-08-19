import geopandas as gpd

from src.algo import SimpleBFS as sbfs

# Select an algorithm from the list of valid algorithms
def selectAlgo(algo: str, population: int, numDists: int, gdf: gpd.GeoDataFrame):
    drawer = None
    # Insert new algorithms into the if-else HERE.
    if algo == "simple":
        drawer =  sbfs.SimpleBFS()
    else:
        print(f"Algorithm {algo} not recognized, districts not drawn.")
        
    
    return drawer