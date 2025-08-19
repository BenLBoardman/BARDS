import geopandas as gpd
import pandas as pd

import json
import time
import os
DATAPATH_IN = "precinctShapefiles/"
DATAPATH_OUT = "output/"

# The lists datasets that can currently be retrieved with getDataSet.
datasets = {
    "census": {'name': 'CENS', 'prefix': 'T', 'fields': ['Total', 'Native', 'Asian', 'Black', 'Pacific', 'White', 'Hispanic']},
    "president": {'name': 'PRES', 'prefix': 'E', 'fields': ['Total', 'Dem', 'Rep']}
}

#Intake a precinct map and prepare it to draw districts
def processIn(state: str, year: int, gdf: gpd.GeoDataFrame):
    population = 0

    censusData = getDataset(year, "census", state, gdf)    
    if censusData[0] == -1:
        print("Census data not found!")
        return -1
    
    gdf['TOTPOP'] = censusData[1]['Total']

    population = gdf['TOTPOP'].sum()
    print("Precinct data loaded... beginning neighbor analysis...")
    startTime = time.time()

    findAllNeighborsGPD(gdf)
    elapsed = round(time.time() - startTime, 3)

    print(f"Neighbor analysis finished in {elapsed} seconds...")
    
    return population

# Get a dataset of a given year and type from the dataframe. This can be used for election data, census data, or VAP data.
# Valid datasets are listed in the datasets dict. Returns a tuple containing (status, dataframe)
def getDataset(year: int, type: str, state: str, gdf: gpd.GeoDataFrame):
    if type not in datasets.keys():
        print("Invalid dataset entry request.")
        return (-1, None)

    dataset = datasets.get(type)
    try:
        datasetName = f"{dataset['prefix']}_{int(year) % 2000}_{dataset['name']}"
        return (0, pd.DataFrame(pd.DataFrame(
            json.loads(str) for str in gdf['datasets'].tolist()
            )[datasetName].tolist(), columns=dataset['fields'])) 
    except:
        print(f"{state} does not include {year} {type} data!")
        return (-1, None)

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
    if not os.path.isdir(DATAPATH_OUT):
        os.mkdir(DATAPATH_OUT)


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