import geopandas as gpd
import pandas as pd

import sys
import json
import time 
DATAPATH_RAW = "data/geojson/"
DATAPATH_PROCESSED = "data/processed/"

state = ""

json.JSONDecoder

def main():
    state = sys.argv[1].upper()
    year = sys.argv[2]

    print(f"Processing data for state: {state}...")
    print(f"Using census data:  {year}...")

    dataFrame = gpd.read_file(f'{DATAPATH_RAW}{state}.geojson')

    print("Features loaded... beginning neighbor analysis...")
    startTime = time.time()
    findAllNeighborsGPD(dataFrame)
    elapsed = time.time() - startTime

    print(f"Neighbor analysis finished in {elapsed} seconds...")
    print(f"Outputting to file...")

    outputFileName = f"{DATAPATH_PROCESSED}{state}_{year}.brd"
    with open(outputFileName, "w") as output:
        writeToBrdArch(output, dataFrame)

    print(f"Processed data written to {outputFileName}...")

def findAllNeighborsGPD(dataFrame: gpd.GeoDataFrame):
    neighborList = []
    for i, feature in dataFrame.iterrows():
        neighbors = dataFrame[dataFrame.geometry.touches(feature.geometry)].index.tolist()
        neighborList.append(neighbors)
    dataFrame['neighbors'] = neighborList
    dataFrame.drop(columns=['geometry'])


def writeToBrdArch(file, dataFrame: gpd.GeoDataFrame):
    # write header
    header = f"||\n"
    file.write(header)
    # write each precinct
    for i, feature in dataFrame.iterrows():
        writePctToFile(file, i, feature)
    pass

def writePctToFile(file, i: int, feature: pd.Series):
    neighbors = ""
    for i in range(0, len(feature.get('neighbors'))):
        neighbor = feature.get('neighbors')[i]
        if i == 0:
            neighbors = neighbor
        neighbors = f"{neighbors}, {neighbor}"
        pass
    file.write(f"{i}|{feature.get('Precinct')}|{feature.get('COUNTY')}|{feature.get('TOTPOP')}|{neighbors}\n")
    pass

# STUB FOR LATER
class State:
    def __init__(self, abbr, pop, year):
        self.abbr = abbr

class Precinct:
    def __init__(self, properties, geometry):
        self.name = properties.get('Precinct')
        self.county = properties.get("COUNTY")
        self.population = properties.get("TOTPOP")

        # Add extra fields once a basic prototype exists

    def setNeighbors(self, neighbors):
        self.neighbors = neighbors

if __name__ == "__main__":
    main()

    