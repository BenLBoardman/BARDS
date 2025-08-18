import geopandas as gpd

from src.algo import SimpleBFS

def selectAlgo(algo: str, population: int, numDists: int, gdf: gpd.GeoDataFrame):
    # Insert new algorithms into the if-else HERE.
    if algo == "simple":
        gdf = SimpleBFS.draw(population, numDists, gdf)
    else:
        print(f"Algorithm {algo} not recognized, districts not drawn.")
        return [-1, None]
    
    return [0, gdf]