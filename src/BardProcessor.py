import geopandas as gpd
import pandas as pd

import json
import time
import os
DATAPATH_IN = "precinctShapefiles/"
DATAPATH_OUT = "output/"
DATAPATH_PROCESSED = "data/processed/"

def fileIntake(state: str, year: int):
    filePath = f"{DATAPATH_IN}{year}/{state}.geojson"
    return gpd.read_file(filePath)

#Intake a precinct map and prepare it to draw districts
def processIn(state: str, year: int, gdf: gpd.GeoDataFrame):
    population = 0

    
    censusData = pd.DataFrame(pd.DataFrame(
            json.loads(str) for str in gdf['datasets'].tolist()
        )['T_20_CENS'].tolist(), columns=['Total', 'Native', 'Asian', 'Black', 'Pacific', 'White', 'Hispanic'])    
    gdf['TOTPOP'] = censusData['Total']

    population = gdf['TOTPOP'].sum()
    print("Precinct data loaded... beginning neighbor analysis...")
    startTime = time.time()

    findAllNeighborsGPD(gdf)
    elapsed = round(time.time() - startTime, 3)

    print(f"Neighbor analysis finished in {elapsed} seconds...")
    
    return population

# Generate neighbor lists for each precinct using GeoPandas
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

# Build a GeoDataFrame corresponding to the geometry of each district, given the DataFrame of precincts including their district 
def buildDistrictGDF(precinctGDF: gpd.GeoDataFrame, distCt: int):
    districtGDF = gpd.GeoDataFrame(columns=['id', 'NAME'], geometry=[])
    for i in range(1, distCt + 1):
      filteredPGDF = precinctGDF.loc[precinctGDF['barddist'] == i]
      districtGDF.loc[len(districtGDF)] = [i, str(i), filteredPGDF.union_all()]
    return districtGDF

# Output a district dataframe to a geoJSON file
def processOut(path: str, gdf: gpd.GeoDataFrame):
    filePath = f"{path}.GeoJSON"
    print(f"Writing to {filePath}...")
    gdf = gdf.set_crs(epsg=4326)
    gdf.to_file(filePath, driver="GeoJSON")

def buildOutputPath(algo: str, name: str, state: str, year: int):
    filePath = f"{DATAPATH_OUT}{algo}"
    if not os.path.isdir(filePath):
        os.mkdir(filePath)

    filePath = f"{filePath}/{year}"
    if not os.path.isdir(filePath):
        os.mkdir(filePath)

    filePath = f"{filePath}/{state}"
    if not os.path.isdir(filePath):
        os.mkdir(filePath)

    index = len(os.listdir(filePath))
    if name == "":
        filePath = f"{filePath}/{state}_{index + 1}"
    else:
        filePath = f"{filePath}/{name}"
    
    return filePath