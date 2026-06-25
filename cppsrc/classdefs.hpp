#include <vector>
#include <string>

#include "geometry.hpp"
#include "dataset.hpp"


class Precinct;
class District;
class State;

class Precinct {
    private:
      int population;
      std::vector<Precinct*> neighbors;
      int index;
      std::string id;
      std::string name;
      State& state;
      District* district;
      Geometry<Precinct> geo;
      std::set<DemographicData<Precinct>> demo;
      std::set<ElectionData<Precinct>> elex;
      
    public:
      Precinct(State& state, std::string json);
      int getPopulation();
      void computeNeighbors();
      std::set<DemographicData<Precinct>> getDemo();
      std::set<ElectionData<Precinct>> getElex();
};


class District {
  private:
    int population;
    int target;
    int id;
    State& state;
    std::vector<Precinct*> precints;
    Geometry<District> geo;  
    std::set<DemographicData<District>> demo;
    std::set<ElectionData<District>> elex;

  public:
    District(State& state, int target);
};

class State {
  private:
    int population;
    int districtCount;
    std::vector<District*> districts;
    std::vector<Precinct*> precincts;
    std::string name;
    std::set<DemographicData<State>> demo;
    std::set<ElectionData<State>> elex;
  
  public:
    State(std::string name, int districtCount);
    void addPrecinct(Precinct& p);
    void finishProcessing();
};