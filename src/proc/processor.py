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

    with open(f'{DATAPATH_RAW}{state}.geojson', 'r') as input:
        data = json.load(input)
        print("JSON Data loaded...")

    for feature in data.get('features'):
        precinct = Precinct(feature.get('properties'), feature.get('geometry'))
        precincts.append(precinct)
    print("Features loaded... beginning neighbor analysis...")
    startTime = time.time()
    getAllNeighbors(precincts)
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

        self.edges = []
        self.makeEdges(geometry.get('coordinates')[0][0])
        self.neighbors = []

    def makeEdges(self, coordinates):
        self.edges = []
        i = 0
        for i in range(0, len(coordinates) - 1):
            self.edges.append((coordinates[i], coordinates[i+1]))
        self.edges.append((coordinates[i], coordinates[0]))

    def isNeighbor(self, other):
        other = cast(Precinct, other)
        for edge in self.edges:
            for otherEdge in other.edges:
                if (edge[0] == otherEdge[0] or edge[0] == otherEdge[1]):
                    self.neighbors.append(other.name)
                    other.neighbors.append(self.name)
                    return 0

    def addNeighbor(self, other):
        self.neighbors.append(other)




def getAllNeighbors(precincts):
    try:
        for i in range(0, len(precincts)):
            print(f"Beginning neighbor analysis on precinct {i + 1} of {len(precincts)}...")
            curr = cast(Precinct, precincts[i])
            for j in range(i + 1, len(precincts)):
                curr.isNeighbor(precincts[j])
    except:
        print(f"ERROR: Neighbor analysis failed on precinct {i + 1} of {len(precincts)} (comparing with precinct {j + 1} of {len(precincts)})")


# STUB FOR LATER
class State:
    def __init__(self, abbr, pop, year):
        self.abbr = abbr


if __name__ == "__main__":
    main()

    