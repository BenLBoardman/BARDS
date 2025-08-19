import geopandas as gpd

import src.BardProcessor as proc
import src.algo.Select as select

import sys

# Commented-out states do not yet have precinct data loaded
defaultCt = {# 'AL':  7, 'AK':  1, 'AZ':  9, 'AR':  4, 'CA': 52,
             # 'CO':  8, 'CT':  5, 'DE':  1, 'FL': 28, 'GA': 14,
             # 'HI':  2, 'ID':  2, 'IL': 17, 'IN':  9, 'IA':  4,
             # 'KS':  4, 'KY':  6, 'LA':  6, 'ME':  2, 'MD':  8,
             # 'MA':  9, 'MI': 13, 'MN':  8, 'MS':  4, 'MO':  8,
               'MT':  2, 'NE':  3, 'NV':  4, 'NH':  2, 'NJ': 12#,
             # 'NM':  3, 'NY': 26, 'NC': 14, 'ND':  1, 'OH': 15,
             # 'OK':  5, 'OR':  6, 'PA': 17, 'RI':  2, 'SC':  7,
             # 'SD':  1, 'TN':  9, 'TX': 38, 'UT':  4, 'VT':  1,
             # 'VA': 11, 'WA': 10, 'WI':  8, 'WV':  2, 'WY':  1
             }

def main():
    args = sys.argv

    # Process args
    if len(args) < 4:
        print("Not enough arguments. Required arguments - python3 BARDS.py <algo> <state> <year>")
        return -1
    
    algo = args[1].lower()

    state = args[2].upper()
    if state not in defaultCt.keys():
        print(f"State {state} not recognized, please try again")
        return -1
    
    population = 0
    year = args[3]
    name = ""

    numDists = defaultCt.get(state)

    # Process Optional Args
    for i in range(4, len(args)):
        if args[i] == '--o':
            if not i == len(args):
                name = args[i+1]
                i += 1
            else:
                print("Optional arg --o should be followed with a name!")
                return -1
        if args[i] == '--n':
            if not i == len(args):
                numDists = int(args[i+1])
                i += 1
            else:
                print("Optional arg --n should be followed with a number of districts!")
                return -1

    print(f"Drawing {numDists} districts for state: {state}...")
    print(f"Using census data:  {year}...")
    print(f"Using algorithm: {algo}...")

    print(f"Loading precinct data...")
    gdf = proc.fileIntake(state, year)

    # Process input data & get neighbors
    population = proc.processIn(state, year, gdf)

    # Insert new algorithms into the if-else in selectAlgo.
    alg = select.selectAlgo(algo, population, numDists, gdf)
    if alg == None:
        return -1
    gdf = alg.draw(population, numDists, gdf)
    
    # Build district geometries
    dists = proc.buildDistrictGDF(gdf, numDists)
    
    filePath = proc.buildOutputPath(algo, name, state, year)
    #Output to file
    proc.processOut(filePath, dists)



if __name__ == "__main__":
    main()

    