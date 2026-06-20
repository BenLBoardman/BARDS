#include <vector>
#include <string>

#include "geometry.cpp"
#include "dataset.hpp"


class Precinct;
class District;
class State;

class Precinct {
    private:
        int population;
        std::vector<Precinct*> neighbors;
        int index;
        std::string name;
        District* district;
        Geometry<Precinct> geo;
        std::set<DemographicData<Precinct>> demo; //not implementing immediately but this seems the best way to do it
        std::set<ElectionData<Precinct>> elex; //see above
};


class District {
  private:
    int population;
    int target;
    int id;
    std::vector<Precinct*> precints;
    Geometry<District> geo();  
    std::set<DemographicData<District>> demo; //not implementing immediately but this seems the best way to do it
    std::set<ElectionData<District>> elex; //see above
};

class State {
  private:
    int population;
    std::vector<District*> districts;
    std::vector <Precinct*> precincts;
    std::string name;
    std::set<DemographicData<State>> demo; //not implementing immediately but this seems the best way to do it
    std::set<ElectionData<State>> elex; //see above
};