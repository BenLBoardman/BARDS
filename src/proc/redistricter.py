import geopandas as gpd

import BardProcessor as proc
import SimpleDistrictAlgorithm as simple

import sys
DATAPATH_RAW = "data/geojson/"
DATAPATH_PROCESSED = "data/processed/"




def main():
    cmd = sys.argv[1].lower()
    state = sys.argv[2].upper()
    population = 0
    year = sys.argv[3]
    numDists = int(sys.argv[4])
    

    print(f"Processing data for state: {state}...")
    print(f"Using census data:  {year}...")

    gdf = gpd.read_file(f'{DATAPATH_RAW}{state}.geojson')

    if cmd == "process":
        proc.process(state, year, True, gdf)

    if cmd == "simple":
        procOut = proc.process(state, year, False, gdf)
        population = procOut[0]
        gdf = procOut[1]
        gdf = simple.draw(population, numDists, gdf)
        dists = simple.buildDistrictGDF(gdf, numDists)



if __name__ == "__main__":
    main()

    