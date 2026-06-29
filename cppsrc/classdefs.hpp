#ifndef PREC_DIST_STATE

#define PREC_DIST_STATE

#include <vector>
#include <string>
#include <algorithm>

#include "geometry.hpp"
#include "dataset.hpp"
#include "json.hpp"

class Precinct;
class District;
class State;

class Precinct {
    private:
      int population;
      std::set<Precinct*> neighbors;
      int index;
      State& state;
      District* district;
      Geometry<Precinct> geo;
      const DemographicData* canonicalDemo;
      const ElectionData* canonicalElex;
      std::set<DemographicData> demo;
      std::set<ElectionData> elex;
      
    public:
      const std::string id;
      const std::string name;

      Precinct(State& state, const JsonValue& json);
      int getPopulation();
      void computeNeighbors();
      void setDistrict(District *d) { district = d; }
      bool isAssigned() { return district != nullptr; }
      Geometry<Precinct> getGeo() { return geo; } //return by value since we should never be modifying precinct geometry once initialized
      std::set<Precinct*> getNeighbors() { return neighbors; }
      std::set<DemographicData> getDemo();
      std::set<ElectionData> getElex();
};


class District {
  private:
    int population;
    
    State& state;
    std::vector<Precinct*> precincts;
    Geometry<District> geo;
    std::string canonicalDemo;
    std::set<DemographicData> demo;
    std::set<ElectionData> elex;

  public:
    const int id;
    const int target;
    District(State& state, int id, int target);
    bool addPrecinct(Precinct* p);
    bool removePrecinct(Precinct* p);
    int getPopulation(){ return population; }
    std::vector<Precinct*> getPrecincts(){ return precincts; }
};

class State {
  private:
    int population;
    int districtCount;
    std::vector<District*> districts;
    std::vector<Precinct*> precincts;
    DemographicData* canonicalDemo;
    ElectionData* canonicalElex;
    std::unordered_map<std::string, DemographicData> demo;
    std::unordered_map<std::string, ElectionData> elex;
    std::set<std::string> datasetNames;
  
  public:
    const std::string abbr;
    const std::string name;

    State(std::string abbr, std::string name, int districtCount);
    void addPrecinct(Precinct& p);
    void finishProcessing();
    void loadDatasets(const JsonValue& json);
    const std::set<std::string>& getDatasetNames() const { return datasetNames; }
    DataSet& getDataSet(const std::string& name);
    const DemographicData* getCanonicalDemo() const { return canonicalDemo; }
    const ElectionData* getCanonicalElex() const { return canonicalElex; }
    std::vector<Precinct*> getPrecincts() { return precincts; }
    std::vector<District*> getDistricts() { return districts; }
    District* getDistrict(int i) { return districts[i-1]; }
};


#endif