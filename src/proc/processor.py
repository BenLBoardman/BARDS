import geopandas as gpd
import pandas as pd

import sys
import json
import time 
DATAPATH_RAW = "data/geojson/"
DATAPATH_PROCESSED = "data/processed/"



json.JSONDecoder

def main():
    state = sys.argv[1].upper()
    population = 0
    year = sys.argv[2]

    print(f"Processing data for state: {state}...")
    print(f"Using census data:  {year}...")

    dataFrame = gpd.read_file(f'{DATAPATH_RAW}{state}.geojson')

    print("Features loaded... beginning neighbor analysis...")
    startTime = time.time()
    population = dataFrame['TOTPOP'].sum()
    findAllNeighborsGPD(dataFrame)
    elapsed = time.time() - startTime

    print(f"Neighbor analysis finished in {elapsed} seconds...")
    print(f"Outputting to file...")

    outputFileName = f"{DATAPATH_PROCESSED}{state}_{year}.brd"
    with open(outputFileName, "w") as output:
        writeToBrdArch(output, population, dataFrame)

    print(f"Processed data written to {outputFileName}...")

def findAllNeighborsGPD(dataFrame: gpd.GeoDataFrame):
    neighborList = []
    for i, feature in dataFrame.iterrows():
        neighbors = dataFrame[dataFrame.geometry.touches(feature.geometry)].index.tolist()
        neighborList.append(neighbors)
    dataFrame['neighbors'] = neighborList
    dataFrame.drop(columns=['geometry'])


def writeToBrdArch(file, population, dataFrame: gpd.GeoDataFrame):
    # TODO - write header
    header = f"ph|{population}|{len(dataFrame)}\n"
    file.write(header)
    # write each precinct
    for i, feature in dataFrame.iterrows():
        writePctToFile(file, i, feature)
    pass

def writePctToFile(file, i: int, feature: pd.Series):
    neighbors = ""
    j = 0
    for j in range(0, len(feature.get('neighbors'))):
        neighbor = feature.get('neighbors')[j]
        if j == 0:
            neighbors = neighbor
        else:
            neighbors = f"{neighbors}, {neighbor}"
    file.write(f"{i}|{feature.get('Precinct')}|{feature.get('COUNTY')}|{feature.get('TOTPOP')}|{j}|{neighbors}\n")

if __name__ == "__main__":
    main()

    