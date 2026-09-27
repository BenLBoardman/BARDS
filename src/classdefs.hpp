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
      bool isAssigned();
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
    /** @brief A pointer to the unassigned district, which this district stores to keep updated.  */
    District *unassigned;
    /** @brief Whether demVoteShare, repVoteShare, demWinProbability, and repWinProbability are up to date with the current canonical election data. */
    bool statsCached;
    /** @brief Cached two-party Democratic vote share of this district's canonical election, in [0, 1]. */
    double demVoteShare;
    /** @brief Cached two-party Republican vote share of this district's canonical election, in [0, 1]. */
    double repVoteShare;
    /** @brief Cached statistically estimated probability that this district votes Democratic, derived from demVoteShare/repVoteShare. */
    double demWinProbability;
    /** @brief Cached statistically estimated probability that this district votes Republican, derived from demVoteShare/repVoteShare. */
    double repWinProbability;
    /**
     * @brief Recompute demVoteShare, repVoteShare, demWinProbability, and repWinProbability from the current canonical election data, if not already cached.
     */
    void updateStats();

  public:
    /** @brief The ideal population for this district, assigned at construction from the statewide population and district count. */
    const int targetPop;
    /**
     * @brief Construct a District, generating empty per-dataset demographic/election data matching the state's loaded dataset schemas.
     * @param state The owning State.
     * @param id The district's identifier string ("0" designates the special unassigned district and sets the global `unassigned` pointer).
     * @param target The target population for this district.
     * @param unassigned The global unassigned district for this State/map.
     */
    District(State& state, std::string id, int target, District *unassigned);
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
    /**
     * @brief Get this district's cached two-party Democratic vote share, recomputing it first if stale.
     * @return The Democratic vote share, in [0, 1].
     */
    double getDemVoteShare();
    /**
     * @brief Get this district's cached two-party Republican vote share, recomputing it first if stale.
     * @return The Republican vote share, in [0, 1].
     */
    double getRepVoteShare();
    /**
     * @brief Get this district's cached statistically estimated probability of voting Democratic, recomputing it first if stale.
     * @return The estimated Democratic win probability.
     */
    double getDemWinProbability();
    /**
     * @brief Get this district's cached statistically estimated probability of voting Republican, recomputing it first if stale.
     * @return The estimated Republican win probability.
     */
    double getRepWinProbability();
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
    /** @brief Whether the statewide report cache fields below are up to date with the current precinct/district assignments. */
    bool reportCached;
    /** @brief Cached result of isComplete(): whether every precinct is assigned to a district. */
    bool complete;
    /** @brief Cached result of whether every district's geometry is contiguous. */
    bool allDistrictsContiguous;
    /** @brief Cached statewide average Polsby-Popper compactness across all districts. */
    double avgPolsbyPopper;
    /** @brief Cached statewide average Reock compactness across all districts. */
    double avgReock;
    /** @brief Cached pointer to the district with the smallest population. */
    District* smallestDistrict;
    /** @brief Cached pointer to the district with the largest population. */
    District* largestDistrict;
    /** @brief Cached statewide population deviation: the population gap between the largest and smallest district, as a fraction of the average district target population. */
    double populationDeviation;
    /** @brief Cached statewide two-party efficiency gap: the difference between each party's wasted votes as a fraction of total two-party votes cast in the canonical election, aggregated across districts. */
    double efficiencyGap;
    /** @brief Cached statewide Democratic vote share of the canonical election, in [0, 1]. */
    double proportionalDemShare;
    /** @brief Cached statewide Republican vote share of the canonical election, in [0, 1]. */
    double proportionalRepShare;
    /** @brief Cached number of seats Democrats would win under a perfectly proportional allocation of the canonical statewide vote. */
    int proportionalDemSeats;
    /** @brief Cached number of seats Republicans would win under a perfectly proportional allocation of the canonical statewide vote. */
    int proportionalRepSeats;
    /** @brief Cached statistically expected number of seats Democrats would win on the current district map. */
    double expectedDemSeats;
    /** @brief Cached statistically expected number of seats Republicans would win on the current district map. */
    double expectedRepSeats;
    /** @brief Cached seat disproportionality: the gap between expectedDemSeats and proportionalDemSeats, as a fraction of districtCount. */
    double seatDisproportionality;
    /**
     * @brief Recompute all statewide report cache fields (completeness, contiguity, compactness, population deviation, efficiency gap, and proportional/expected seat counts) in a single pass over precincts and districts, if not already cached.
     */
    void updateReportCache();
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
    /**
     * @brief Mark the statewide report cache (and, transitively, per-district cached partisan stats) as stale, forcing the next report call to recompute it. Called whenever a precinct is assigned to or removed from a district.
     */
    void invalidateReportCache() { reportCached = false; }
    /**
     * @brief Clear map information from a state by unassigning all Precincts from their Districts.
     */
    void clearMap();
 
 
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
    //cached report value accessors (recompute the report cache first if stale; see updateReportCache())
    /**
     * @brief Get whether every district in the state is contiguous, recomputing the report cache first if stale.
     * @return True if every district's geometry is contiguous.
     */
    bool isContiguous();
    /**
     * @brief Get the district with the smallest population, recomputing the report cache first if stale.
     * @return Pointer to the smallest district.
     */
    District* getSmallestDistrict();
    /**
     * @brief Get the district with the largest population, recomputing the report cache first if stale.
     * @return Pointer to the largest district.
     */
    District* getLargestDistrict();
    /**
     * @brief Get the statewide population deviation between the largest and smallest district, recomputing the report cache first if stale.
     * @return The population deviation, as a fraction of the average district target population.
     */
    double getPopulationDeviation();
    /**
     * @brief Get the statewide two-party efficiency gap, recomputing the report cache first if stale.
     * @return The efficiency gap; positive values are biased towards Democrats, negative towards Republicans.
     */
    double getEfficiencyGap();
    /**
     * @brief Get the statewide Democratic vote share of the canonical election, recomputing the report cache first if stale.
     * @return The Democratic vote share, in [0, 1].
     */
    double getProportionalDemShare();
    /**
     * @brief Get the statewide Republican vote share of the canonical election, recomputing the report cache first if stale.
     * @return The Republican vote share, in [0, 1].
     */
    double getProportionalRepShare();
    /**
     * @brief Get the number of seats Democrats would win under a perfectly proportional allocation of the canonical statewide vote, recomputing the report cache first if stale.
     * @return The proportional Democratic seat count.
     */
    int getProportionalDemSeats();
    /**
     * @brief Get the number of seats Republicans would win under a perfectly proportional allocation of the canonical statewide vote, recomputing the report cache first if stale.
     * @return The proportional Republican seat count.
     */
    int getProportionalRepSeats();
    /**
     * @brief Get the statistically expected number of seats Democrats would win on the current district map, recomputing the report cache first if stale.
     * @return The expected Democratic seat count.
     */
    double getExpectedDemSeats();
    /**
     * @brief Get the statistically expected number of seats Republicans would win on the current district map, recomputing the report cache first if stale.
     * @return The expected Republican seat count.
     */
    double getExpectedRepSeats();
    /**
     * @brief Get the statewide seat disproportionality between expected and proportional Democratic seats, recomputing the report cache first if stale.
     * @return The seat disproportionality, as a fraction of the district count; positive values favor Democrats.
     */
    double getSeatDisproportionality();
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
