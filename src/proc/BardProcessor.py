import geopandas as gpd
import pandas as pd


import time 
DATAPATH_RAW = "data/geojson/"
DATAPATH_PROCESSED = "data/processed/"


def process(state: str, year: int, toFile: bool, gdf: gpd.GeoDataFrame):
    population = 0

    print("Features loaded... beginning neighbor analysis...")
    startTime = time.time()
    population = gdf['TOTPOP'].sum()
    findAllNeighborsGPD(gdf)
    elapsed = time.time() - startTime

    print(f"Neighbor analysis finished in {elapsed} seconds...")

    if(toFile):
        print(f"Outputting to file...")

        outputFileName = f"{DATAPATH_PROCESSED}{state}_{year}.brd"
        with open(outputFileName, "w") as output:
            writeToBrdArch(output, population, gdf)

        print(f"Processed data written to {outputFileName}...")
    
    return (population, gdf)

def findAllNeighborsGPD(gdf: gpd.GeoDataFrame):
    neighborList = []
    indexList = []
    for i, feature in gdf.iterrows():
        neighbors = gdf[gdf.geometry.intersects(feature.geometry)].index.tolist()
        neighbors.remove(i)
        neighborList.append(neighbors)
        indexList.append(i)
    gdf['neighbors'] = neighborList
    gdf['index'] = indexList

def writeToBrdArch(file, population, gdf: gpd.GeoDataFrame):
    # TODO - write header
    header = f"ph|{population}|{len(gdf)}\n"
    file.write(header)
    # write each precinct
    for i, feature in gdf.iterrows():
        writePctToFile(file, i, feature)
    pass

def writePctToFile(file, i: int, feature: pd.Series):
    neighbors = ""
    j = 0
    numNeighbors = len(feature.get('neighbors'))
    for j in range(0, numNeighbors):
        neighbor = feature.get('neighbors')[j]
        if j == 0:
            neighbors = neighbor
        else:
            neighbors = f"{neighbors}, {neighbor}"
    file.write(f"{i}|{feature.get('Precinct')}|{feature.get('COUNTY')}|{feature.get('TOTPOP')}|{numNeighbors}|{neighbors}\n")
    