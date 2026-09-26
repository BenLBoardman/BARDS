/**
 * @file classdefs.hpp
 * @brief Core domain classes representing the electoral map hierarchy:
 *        Precinct, District, and State.
 *
 * Precinct is the smallest unit of geography, carrying demographic and
 * election data along with geometry. District aggregates Precincts into a
 * single electoral district. State owns all Precincts and Districts for a
 * state and drives dataset loading, processing, and statewide reporting.
 */
#pragma once


#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <functional>

#include "geometry.hpp"
#include "dataset.hpp"
#include "util/json.hpp"
#include "electoralentity.hpp"

class Precinct;
class District;
class State;

/** @brief Shared random-number source used for random precinct/neighbor selection throughout the program. */
extern std::random_device rd;
/** @brief Pointer to the special "unassigned" District (id "0") that holds precincts not currently assigned to a real district. 
 *  Exists as a global to make access easier. */
extern District *unassigned;

/**
 * @class Precinct
 * @brief The smallest electoral geography unit. Carries per-dataset
 *        demographic/election data and geometry inherited from
 *        ElectoralEntity, and additionally tracks adjacency to other
 *        precincts and its owning District.
 */
class Precinct : public ElectoralEntity {
    private:
      /** @brief Precincts that share a boundary edge with this precinct, as computed by computeNeighbors(). */
      std::vector<Precinct*> neighbors;
      /** @brief The State this precinct belongs to; used to resolve dataset schemas at construction time. */
      State& state;
      /** @brief The District this precinct is currently assigned to (may be the "unassigned" district or nullptr before any assignment). */
      District* district;
      
    public:
      /** @brief Always true for Precinct instances; lets code distinguish Precinct from other ElectoralEntity subclasses without RTTI. */
      const bool isPrecinct = true;
       /**
       * @brief Construct a Precinct from its GeoJSON feature representation, populating demographic/election data and geometry.
       * @param state The owning State, used to resolve dataset schemas and canonical data sets.
       * @param json The GeoJSON feature for this precinct, containing "properties" (id, name, per-dataset values) and "geometry".
       */
      Precinct(State& state, const JsonValue& json);
      /**
       * @brief Populate the neighbors list by inspecting the owning Geometry of each boundary GeoLine.
       */
      void computeNeighbors();
      /**
       * @brief Set the district this precinct belongs to.
       * @param d The new owning District (may be the "unassigned" district or nullptr).
       */
      void setDistrict(District *d) { district = d; }
      /**
       * @brief Determine whether this precinct has been assigned to a real (non-"unassigned") district.
       * @return True if assigned to a non-null district other than the unassigned district.
       */
      bool isAssigned() { return district != nullptr && district != unassigned;  }
      /**
       * @brief Get the list of neighboring precincts.
       * @return A copy of the neighbors vector.
       */
      std::vector<Precinct*> getNeighbors() { return neighbors; }
      /**
       * @brief Get a random neighboring precinct.
       * @param requireUnassigned If true, keep sampling until an unassigned neighbor is found.
       * @return Pointer to the selected neighboring precinct.
       */
      Precinct *getRandNeighbor(bool requireUnassigned);
      /**
       * @brief Randomly shuffle the order of the neighbors vector in-place.
       */
      void permuteNeighbors();
};

/**
 * @class District
 * @brief An aggregation of Precincts forming one electoral district, with
 *        merged demographic/election data and geometry maintained as
 *        precincts are added and removed.
 */
class District : public ElectoralEntity {
  private:    
    /** @brief The State this district belongs to. */
    State& state;
    /** @brief The precincts currently assigned to this district. */
    std::vector<Precinct*> precincts;


  public:
    /** @brief The ideal population for this district, assigned at construction from the statewide population and district count. */
    const int targetPop;
    /**
     * @brief Construct a District, generating empty per-dataset demographic/election data matching the state's loaded dataset schemas.
     * @param state The owning State.
     * @param id The district's identifier string ("0" designates the special unassigned district and sets the global `unassigned` pointer).
     * @param target The target population for this district.
     */
    District(State& state, std::string id, int target);
    /**
     * @brief Assign a precinct to this district: merges its demographic/election data and geometry in, and removes it from the unassigned district first if needed.
     * @param p The precinct to add.
     * @return True if the precinct was added; false if it was already assigned to a district.
     */
    bool addPrecinct(Precinct* p);
    /**
     * @brief Remove a precinct from this district: unmerges its demographic/election data and geometry, and returns it to the unassigned district.
     * @param p The precinct to remove.
     * @return True if the precinct was removed; false if it was not assigned to this district.
     */
    bool removePrecinct(Precinct* p);
    /**
     * @brief Get the precincts currently assigned to this district.
     * @return A copy of the precincts vector.
     */
    std::vector<Precinct*> getPrecincts(){ return precincts; }
    /**
     * @brief Determine whether this district's geometry is contiguous.
     * @return True if the district's geometry forms a single contiguous boundary.
     */
    bool isContiguous() {return geo.isContiguous(); }
    /**
     * @brief Compute this district's Polsby-Popper compactness score.
     * @return The Polsby-Popper score.
     */
    double compactnessPolsbyPopper(){ return geo.getPolsbyPopper(); }
    /**
     * @brief Compute this district's Reock compactness score.
     * @return The Reock score.
     */
    double compactnessReock(){ return geo.getReock(); }
    /**
     * @brief Compute this district's population deviation from its target as a signed fraction.
     * @return (population - targetPop) / targetPop.
     */
    double popDeviation() { return 1.0*(population-targetPop)/targetPop; }
    /**
     * @brief Determine whether this is the special unassigned district.
     * @return True if this district's id is "0".
     */
    bool isUnassigned() { return id.compare("0") == 0; }
    /**
     * @brief Compute this district's population as a fraction of its target.
     * @return population / targetPop.
     */
    double getDeviation() { return 1.0 * population / targetPop; }
};

/**
 * @class State
 * @brief Top-level container owning all precincts and districts for a
 *        state. Responsible for dataset loading, precinct
 *        assignment/selection helpers, map finalization, and statewide
 *        report generation (report methods implemented in reports.cpp).
 */
class State : public ElectoralEntity {
  private:
    /** @brief The number of districts the state should be divided into. */
    int districtCount;
    /** @brief The average population per precinct, recomputed each time a precinct is added. */
    int averagePrecinctPop;
    /** @brief All districts in this state, including the special unassigned district. */
    std::vector<District*> districts;
    /** @brief All precincts in this state. */
    std::vector<Precinct*> precincts;
    /** @brief Names of all datasets (demographic and election) loaded for this state. */
    std::set<std::string> datasetNames;
    /** @brief The special district (id "0") holding precincts not yet assigned to a real district. */
    District* unassigned;
  
  public:
    /**
     * @brief Construct a State.
     * @param id The state's identifier.
     * @param name The state's display name.
     * @param districtCount The number of districts to divide the state into.
     */
    State(std::string id, std::string name, int districtCount);
    /**
     * @brief Register a precinct with the state: adds it to the unassigned district, and updates statewide population and average precinct population.
     * @param p The precinct to add.
     */
    void addPrecinct(Precinct& p);
    /**
     * @brief Get a single random precinct from the state.
     * @param requireUnassigned If true, keep sampling until an unassigned precinct is found.
     * @return Pointer to the selected precinct.
     */
    Precinct* getRandPrecinct(bool requireUnassigned);
    /**
     * @brief Get an arbitrary number of unique randomly-selected precincts conforming to a custom predicate.
     * @param requireUnassigned If true, will additionally require that the precincts selected are not assigned to any district.
     * @param cnt The number of precincts to return.
     * @param customPred A custom predicate taking as parameters a pointer to the calling State, the list of precincts already selected and the most recently selected candidate precinct.
     *    If not passed in, it will always return "true".
     * @return A vector of pointers to the selected precincts.
     */
    std::vector<Precinct*> getRandPrecincts(bool requireUnassigned, int cnt, 
      const std::function<bool(State*, Precinct*, std::vector<Precinct*>)>& customPred = [](State *s, Precinct *p, std::vector<Precinct*> precs){return true; });
    /**
     * @brief Finalize state setup after all precincts are loaded: creates districts sized to a target population, then computes precinct adjacency (neighbors) for every precinct.
     */
    void finishProcessing();
    /**
     * @brief Load dataset schemas from JSON, prompting the user (via stdin) to select the canonical demographic and election datasets, and initializes the special unassigned district.
     * @param json A JSON object mapping dataset names to their schema definitions.
     */
    void loadDatasets(const JsonValue& json);
    /**
     * @brief Get the names of all datasets loaded for this state.
     * @return A const reference to the set of dataset names.
     */
    const std::set<std::string>& getDatasetNames() const { return datasetNames; }
    /**
     * @brief Look up a loaded dataset schema by name.
     * @param name The dataset name.
     * @return A reference to the matching DataSet.
     * @throws std::runtime_error if no dataset with that name exists.
     */
    DataSet& getDataSet(const std::string& name);
    /**
     * @brief Get all precincts in the state.
     * @return A copy of the precincts vector.
     */
    std::vector<Precinct*> getPrecincts() { return precincts; }
    /**
     * @brief Get all districts in the state.
     * @return A copy of the districts vector.
     */
    std::vector<District*> getDistricts() { return districts; }
    /**
     * @brief Get a district by its 1-based index.
     * @param i The 1-based district index.
     * @return Pointer to the corresponding district.
     */
    District* getDistrict(int i) { return districts[i-1]; }
    /**
     * @brief Get the average population per precinct.
     * @return The average precinct population.
     */
    int getAveragePrecinctPop() { return averagePrecinctPop; }
 
 
 
    //report functions (defined in reports.cpp)
    /**
     * @brief Compute the statewide average Polsby-Popper compactness across all districts.
     * @return The average Polsby-Popper score.
     */
    double compactnessPolsbyPopper();
    /**
     * @brief Compute the statewide average Reock compactness across all districts.
     * @return The average Reock score.
     */
    double compactnessReock();
    /**
     * @brief Determine whether every precinct in the state has been assigned to a district.
     * @return True if all precincts are assigned.
     */
    bool isComplete();
    /**
     * @brief Print a report analyzing statewide population deviation between the largest and smallest districts, including whether it likely meets the typical legal threshold.
     */
    void populationDeviationAnalysis();
    /**
     * @brief Print a report on the statewide two-party efficiency gap: the difference between each party's wasted votes as a fraction of the total two-party votes cast in the canonical election, aggregated across districts.
     */
    void efficiencyGapAnalysis();
    /**
     * @brief Print a report comparing the proportional statewide seat allocation for the canonical election to the statistically expected seat count on the current district map, and the resulting disproportionality.
     */
    void partisanExpectedSeatAnalysis();
    /**
     * @brief Print a report on whether the map is complete (all precincts assigned), whether every district is contiguous, and statewide compactness scores.
     */
    void contiguityCompactnessAnalysis();
    /**
     * @brief Print a detailed per-district report: contiguity, compactness, population deviation from target, and estimated partisan lean.
     */
    void districtByDistrictAnalysis();
    /**
     * @brief Print the full suite of statewide and district-by-district reports in sequence.
     */
    void fullReport();
};
