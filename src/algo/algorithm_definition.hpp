/**
 * @file algorithm_definition.hpp
 * @brief Base class and concrete implementations for district-drawing
 *        algorithms, plus the registry (algos) and loader
 *        (loadAlgorithms) used to make them selectable at runtime.
 */
#pragma once

#include "../classdefs.hpp"
#include "../geometry.hpp"
#include "../dataset.hpp"

#include <vector>


/**
 * @brief Instantiate one instance of each available DistrictAlgorithm and register them in the global algos list.
 */
void loadAlgorithms();

/**
 * An extendable base to implement an algorithm to draw a district. Includes an empty constructor and an entry point.
 */
class DistrictAlgorithm {
    public:
        /** @brief The algorithm's short display name. */
        const std::string name;
        /** @brief A human-readable description of what the algorithm does. */
        const std::string desc;
        /**
         * @brief Construct a DistrictAlgorithm with a name and description.
         * @param n The algorithm's short display name.
         * @param d A human-readable description of the algorithm.
         */
        DistrictAlgorithm(std::string n, std::string d) : name(n), desc(d) {}
        /**
         * @brief Run this algorithm, assigning every precinct in the state to a district.
         * @param s The state whose precincts should be districted.
         */
        virtual void drawMap(State& s) = 0;
};

/** @brief Global registry of all available DistrictAlgorithm instances, populated by loadAlgorithms(). */
extern std::vector<DistrictAlgorithm*> algos;

/**
 * @class MonoDistrictTest
 * @brief Test algorithm that assigns every precinct in the state to a single district.
 */
class MonoDistrictTest : public DistrictAlgorithm {
    public:
        /** @brief Construct the MonoDistrictTest algorithm with its fixed name and description. */
        MonoDistrictTest() : DistrictAlgorithm("SingleDistrict", "Test Algorithm that draws a single district") {}
        /**
         * @brief Assign every precinct in the state to one district.
         * @param s The state whose precincts should be districted.
         */
        void drawMap(State& s);
};

/**
 * @class SimpleBFS
 * @brief Builds districts one at a time by breadth-first search outward from a random starting precinct.
 */
class SimpleBFS : public DistrictAlgorithm {
    public:
        /** @brief Construct the SimpleBFS algorithm with its fixed name and description. */
        SimpleBFS() : DistrictAlgorithm("SimpleBFS", "Algorithm that sequentially builds districts by BFS-ing from a random starting point") {}
        /**
         * @brief Sequentially fill each district by breadth-first search from a random unassigned precinct.
         * @param s The state whose precincts should be districted.
         */
        void drawMap(State& s);
};

/**
 * @class SimpleDFS
 * @brief Builds districts one at a time by depth-first search outward from a random starting precinct.
 */
class SimpleDFS : public DistrictAlgorithm {
    private:
        /** @brief Index of the district currently being filled. */
        int distIndex;
        /** @brief The district currently being filled by the depth-first traversal. */
        District *currDist;
        /** @brief The list of districts being built up, in fill order. */
        std::vector<District *> dists;
        /** @brief The state being districted, retained across recursive process() calls. */
        State *st;
    public:
        /** @brief Construct the SimpleDFS algorithm with its fixed name and description. */
        SimpleDFS() : DistrictAlgorithm("SimpleDFS", "Algorithm that sequentially builds districts by DFS-ing from a random starting point") {}
        /**
         * @brief Recursively visit a precinct as part of the depth-first fill of currDist, assigning it and continuing to its unassigned neighbors.
         * @param prec The precinct to visit and assign.
         */
        void process(Precinct *prec);
        /**
         * @brief Sequentially fill each district by depth-first search from a random unassigned precinct.
         * @param s The state whose precincts should be districted.
         */
        void drawMap(State& s);
};

/**
 * @class MultiBFS
 * @brief Builds all districts simultaneously by breadth-first search, growing each district outward from its own random starting precinct in parallel.
 */
class MultiBFS : public DistrictAlgorithm {
    public:
        /** @brief Construct the MultiBFS algorithm with its fixed name and description. */
        MultiBFS() : DistrictAlgorithm("MultiBFS", "Algorithm that independently builds districts by BFS-ing from a random starting point per-district") {}
        /**
         * @brief Grow all districts in parallel via breadth-first search, each from its own random starting precinct.
         * @param s The state whose precincts should be districted.
         */
        void drawMap(State& s);
};