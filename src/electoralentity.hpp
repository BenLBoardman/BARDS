/**
 * @file electoralentity.hpp
 * @brief Common base class for objects that participate in the electoral
 *        hierarchy (precincts, districts, states) and carry population,
 *        per-dataset demographic/election data, and geometry.
 */
#pragma once 

#include <set>

#include "dataset.hpp"
#include "geometry.hpp"


/**
 * @class ElectoralEntity
 * @brief Base class providing shared state for any geographic/political
 *        unit: population, canonical demographic/election data pointers,
 *        per-dataset demographic/election maps, and a Geometry.
 *        Subclassed by Precinct, District, and State.
 */
class ElectoralEntity {
  protected:
    /** @brief The total population of this entity, per the canonical demographic data set. */
    int population;
    /** @brief Index field reserved for entity ordering/lookup. */
    int index;
    /** @brief Pointer to this entity's data under the state's canonical demographic data set. */
    const DemographicData* canonicalDemo;
    /** @brief Pointer to this entity's data under the state's canonical election data set. */
    const ElectionData* canonicalElex;
    /** @brief This entity's demographic data, keyed by dataset name. */
    std::unordered_map<std::string, DemographicData> demo;
    /** @brief This entity's election data, keyed by dataset name. */
    std::unordered_map<std::string, ElectionData> elex;
    /** @brief This entity's geometry (boundary lines and derived shape properties). */
    Geometry geo;

  public:
    /** @brief This entity's identifier. */
    const std::string id;
    /** @brief This entity's display name. */
    const std::string name;
    /** @brief Whether this entity is a Precinct; overridden to true in the Precinct subclass. */
    const bool isPrecinct = false;
    /** @brief Virtual destructor, allowing safe deletion through base-class pointers. */
    virtual ~ElectoralEntity() = default;
    /**
     * @brief Construct an ElectoralEntity with an id and name, initializing its geometry to be owned by this entity.
     * @param id The entity's identifier.
     * @param name The entity's display name.
     */
    ElectoralEntity(std::string id, std::string name) : geo(this), id(id), name(name) {};
    /**
     * @brief Get this entity's demographic data map.
     * @return A reference to the map of dataset name to DemographicData.
     */
    std::unordered_map<std::string, DemographicData>& getDemo() { return demo; }
    /**
     * @brief Get this entity's election data map.
     * @return A reference to the map of dataset name to ElectionData.
     */
    std::unordered_map<std::string, ElectionData>& getElex() { return elex; }
    /**
     * @brief Get this entity's canonical election data.
     * @return Pointer to the canonical ElectionData.
     */
    const ElectionData* getCanonicalElex() const { return canonicalElex; }
    /**
     * @brief Get this entity's canonical demographic data.
     * @return Pointer to the canonical DemographicData.
     */
    const DemographicData* getCanonicalDemo() const { return canonicalDemo; }
    /**
     * @brief Get this entity's total population.
     * @return The population.
     */
    int getPopulation() { return population; };
    /**
     * @brief Get this entity's geometry.
     * @return A reference to the Geometry.
     */
    Geometry& getGeo() { return geo; }
};