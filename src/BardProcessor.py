import geopandas as gpd
import pandas as pd

import json
import time 
DATAPATH_IN = "data/in/"
DATAPATH_OUT = "data/out/"
DATAPATH_PROCESSED = "data/processed/"


def processIn(state: str, year: int, toFile: bool, gdf: gpd.GeoDataFrame):
    population = 0

    
    cvap = pd.DataFrame(pd.DataFrame(
            json.loads(str) for str in gdf['datasets'].tolist()
        )['T_20_CENS'].tolist(), columns=['Total', 'Native', 'Asian', 'Black', 'Pacific', 'White', 'Hispanic'])    
    gdf['TOTPOP'] = cvap['Total']

    population = gdf['TOTPOP'].sum()
    print("Precinct data loaded... beginning neighbor analysis...")
    startTime = time.time()

    findAllNeighborsGPD(gdf)
    elapsed = time.time() - startTime

    print(f"Neighbor analysis finished in {elapsed} seconds...")
    
    return population

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

def buildDistrictGDF(precinctGDF: gpd.GeoDataFrame, distCt: int):
    districtGDF = gpd.GeoDataFrame(columns=['id', 'NAME'], geometry=[])
    for i in range(1, distCt + 1):
      filteredPGDF = precinctGDF.loc[precinctGDF['barddist'] == i]
      districtGDF.loc[len(districtGDF)] = [i, str(i), filteredPGDF.union_all()]
    return districtGDF

def processOut(fileName: str, gdf: gpd.GeoDataFrame):
    filePath = f"{DATAPATH_OUT}{fileName}.GeoJSON"
    print(f"Writing to {filePath}...")
    gdf = gdf.set_crs(epsg=4326)
    gdf.to_file(filePath, driver="GeoJSON")