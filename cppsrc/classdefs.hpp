#include <vector>
#include <string>

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
      std::string id;
      std::string name;
      State& state;
      District* district;
      Geometry<Precinct> geo;
      const DemographicData* canonicalDemo;
      const ElectionData* canonicalElex;
      std::set<DemographicData> demo;
      std::set<ElectionData> elex;
      
    public:
      Precinct(State& state, const JsonValue& json);
      int getPopulation();
      void computeNeighbors();
      std::set<DemographicData> getDemo();
      std::set<ElectionData> getElex();
};


class District {
  private:
    int population;
    int target;
    int id;
    State& state;
    std::vector<Precinct*> precints;
    Geometry<District> geo;
    std::string canonicalDemo;
    std::set<DemographicData> demo;
    std::set<ElectionData> elex;

  public:
    District(State& state, int target);
};

class State {
  private:
    int population;
    int districtCount;
    std::vector<District*> districts;
    std::vector<Precinct*> precincts;
    std::string abbr;
    std::string name;
    DemographicData* canonicalDemo;
    ElectionData* canonicalElex;
    std::unordered_map<std::string, DemographicData> demo;
    std::unordered_map<std::string, ElectionData> elex;
    std::set<std::string> datasetNames;
  
  public:
    State(std::string abbr, std::string name, int districtCount);
    void addPrecinct(Precinct& p);
    void finishProcessing();
    void loadDatasets(const JsonValue& json);
    const std::set<std::string>& getDatasetNames() const { return datasetNames; }
    DataSet& getDataSet(const std::string& name);
    const DemographicData* getCanonicalDemo() const { return canonicalDemo; }
    const ElectionData* getCanonicalElex() const { return canonicalElex; }
};