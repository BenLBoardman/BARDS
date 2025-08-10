import geopandas as gpd

import sys
import json
from typing import cast
import time 
DATAPATH_RAW = "data/geojson/"
DATAPATH_PROCESSED = "data/processed/"


state = ""


json.JSONDecoder

def main():
    state = sys.argv[1].upper()
    year = sys.argv[2]

    precincts = []
    print(f"Processing data for state: {state}...")
    print(f"Using census data:  {year}...")

    dataFrame = gpd.read_file(f'{DATAPATH_RAW}{state}.geojson')

    with open(f'{DATAPATH_RAW}{state}.geojson', 'r') as input:
        data = json.load(input)
        print("JSON Data loaded...")

    for feature in data.get('features'):
        precinct = Precinct(feature.get('properties'), feature.get('geometry'))
        precincts.append(precinct)
    print("Features loaded... beginning neighbor analysis...")
    startTime = time.time()
    neighborDict = findAllNeighborsGPD(precincts, dataFrame)
    elapsed = time.time() - startTime

    print(f"Neighbor analysis finished in {elapsed} seconds...")
    print(f"Outputting to file...")

    with open(f"{DATAPATH_PROCESSED}{state}_{year}.json", "w") as output:
        json.dump(precincts, output, default=lambda x: x.__dict__)

    print(f"Processed data written to {DATAPATH_PROCESSED}{state}_{year}.json...")



class Precinct:
    def __init__(self, properties, geometry):
        self.name = properties.get('Precinct')
        self.county = properties.get("COUNTY")
        self.population = properties.get("TOTPOP")

        # Add extra fields once a basic prototype exists

    def setNeighbors(self, neighbors):
        self.neighbors = neighbors

def findAllNeighborsGPD(precincts, dataFrame):
    for i, feature in dataFrame.iterrows():
        neighbors = dataFrame[dataFrame.geometry.touches(feature.geometry)].Precinct.tolist()
        precincts[i].setNeighbors(neighbors)

# STUB FOR LATER
class State:
    def __init__(self, abbr, pop, year):
        self.abbr = abbr


if __name__ == "__main__":
    main()

    