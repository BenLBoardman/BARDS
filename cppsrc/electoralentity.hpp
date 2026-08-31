#pragma once 

#include <set>

#include "dataset.hpp"
#include "geometry.hpp"


class ElectoralEntity {
  protected:
    int population;
    int index;
    const DemographicData* canonicalDemo;
    const ElectionData* canonicalElex;
    std::unordered_map<std::string, DemographicData> demo;
    std::unordered_map<std::string, ElectionData> elex;
    Geometry geo;

  public:
    const bool isPrecinct = false;
    virtual ~ElectoralEntity() = default;
    ElectoralEntity(std::string id, std::string name) : name(name), id(id), geo(*this) {};
    const std::string id;
    const std::string name;
    std::unordered_map<std::string, DemographicData>& getDemo() { return demo; }
    std::unordered_map<std::string, ElectionData>& getElex() { return elex; }
    const ElectionData* getCanonicalElex() const { return canonicalElex; }
    const DemographicData* getCanonicalDemo() const { return canonicalDemo; }
    int getPopulation() { return population; };
    Geometry& getGeo() { return geo; }
};