#pragma once


#include <vector>
#include <string>
#include <algorithm>
#include <random>

#include "geometry.hpp"
#include "dataset.hpp"
#include "json.hpp"
#include "electoralentity.hpp"

class Precinct;
class District;
class State;


extern std::random_device rd;



class Precinct : public ElectoralEntity {
    private:
      std::vector<Precinct*> neighbors;
      State& state;
      District* district;
      
    public:
      Precinct(State& state, const JsonValue& json);
      void computeNeighbors();
      void setDistrict(District *d) { district = d; }
      bool isAssigned() { return district != nullptr; }
      std::vector<Precinct*> getNeighbors() { return neighbors; }
      Precinct *getRandNeighbor(bool requireUnassigned);
};


class District : public ElectoralEntity {
  private:    
    State& state;
    std::vector<Precinct*> precincts;


  public:
    const int targetPop;
    District(State& state, std::string id, int target);
    bool addPrecinct(Precinct* p);
    bool removePrecinct(Precinct* p);
    std::vector<Precinct*> getPrecincts(){ return precincts; }
    bool isContiguous() {return geo.isContiguous(); }
    double compactnessPolsbyPopper(){ return geo.getPolsbyPopper(); }
    double compactnessReock(){ return geo.getReock(); }
};

class State : public ElectoralEntity {
  private:
    int districtCount;
    std::vector<District*> districts;
    std::vector<Precinct*> precincts;
    std::unordered_map<std::string, DemographicData> demo;
    std::unordered_map<std::string, ElectionData> elex;
    std::set<std::string> datasetNames;
  
  public:

    State(std::string id, std::string name, int districtCount);
    void addPrecinct(Precinct& p);
    Precinct* getRandPrecinct(bool requireUnassigned);
    void finishProcessing();
    void loadDatasets(const JsonValue& json);
    const std::set<std::string>& getDatasetNames() const { return datasetNames; }
    DataSet& getDataSet(const std::string& name);
    std::vector<Precinct*> getPrecincts() { return precincts; }
    std::vector<District*> getDistricts() { return districts; }
    District* getDistrict(int i) { return districts[i-1]; }
    double compactnessPolsbyPopper();
    double compactnessReock();
};
