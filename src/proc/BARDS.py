import geopandas as gpd

import BardProcessor as proc
import SimpleDistrictAlgorithm as simple

import sys

# Commented-out states do not yet have precinct data in the app
defaultCt = {# 'AL':  7, 'AK':  1, 'AZ':  9, 'AR':  4, 'CA': 52,
             # 'CO':  8, 'CT':  5, 'DE':  1, 'FL': 28, 'GA': 14,
             # 'HI':  2, 'ID':  2, 'IL': 17, 'IN':  9, 'IA':  4,
             # 'KS':  4, 'KY':  6, 'LA':  6, 'ME':  2, 'MD':  8,
             # 'MA':  9, 'MI': 13, 'MN':  8, 'MS':  4, 'MO':  8,
             # 'MT':  2, 'NE':  3, 'NV':  4, 
             'NH':  2
             #, 'NJ': 11,
             # 'NM':  3, 'NY': 26, 'NC': 14, 'ND':  1, 'OH': 15,
             # 'OK':  5, 'OR':  6, 'PA': 17, 'RI':  2, 'SC':  7,
             # 'SD':  1, 'TN':  9, 'TX': 38, 'UT':  4, 'VT':  1,
             # 'VA': 11, 'WA': 10, 'WI':  8, 'WV':  2, 'WY':  1
             }

def main():
    if len(sys.argv) < 4:
        print("Not enough arguments. Required arguments - python3 BARDS.py <algo> <state> <year>")
        return -1
    
    algo = sys.argv[1].lower()

    state = sys.argv[2].upper()
    if state not in defaultCt.keys():
        print(f"State {state} not recognized, please try again")
        return -1
    
    population = 0
    year = sys.argv[3]

    if len(sys.argv) == 5:
        numDists = int(sys.argv[4])
    else: 
        numDists = defaultCt.get(state)

    print(f"Drawing {numDists} districts for state: {state}...")
    print(f"Using census data:  {year}...")
    print(f"Using algorithm: {algo}...")

    print(f"Loading precinct data...")
    gdf = gpd.read_file(f'{proc.DATAPATH_IN}{state}_{year}.geojson')

    # Process input data & get neighbors
    population = proc.processIn(state, year, False, gdf)

    # Insert new algorithms into the if-else HERE.
    if algo == "simple":
        gdf = simple.draw(population, numDists, gdf)
    else:
        print(f"Algorithm {algo} not recognized, districts not drawn.")
        return -1
    
    # Build district geometries
    dists = proc.buildDistrictGDF(gdf, numDists)
    
    #Output to file
    proc.processOut(f"{state}_{year}-{algo}", dists)



if __name__ == "__main__":
    main()

    