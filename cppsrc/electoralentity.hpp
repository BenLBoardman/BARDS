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
    std::set<DemographicData> demo;
    std::set<ElectionData> elex;
    Geometry geo;

  public:
    virtual ~ElectoralEntity() = default;
    ElectoralEntity(std::string id, std::string name) : name(name), id(id), geo(*this) {};
    const std::string id;
    const std::string name;
    const std::set<DemographicData> &getDemo() const { return demo; };
    const std::set<ElectionData> &getElex() const { return elex; };
    const ElectionData* getCanonicalElex() const { return canonicalElex; }
    const DemographicData* getCanonicalDemo() const { return canonicalDemo; }
    int getPopulation() { return population; };
    Geometry& getGeo() { return geo; }
};