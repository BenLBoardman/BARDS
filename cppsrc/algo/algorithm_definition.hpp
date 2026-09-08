#pragma once

#include "../classdefs.hpp"
#include "../geometry.hpp"
#include "../dataset.hpp"

#include <vector>

void loadAlgorithms();

/**
 * An extendable base to implement an algorithm to draw a district. Includes an empty constructor and an entry point.
 */
class DistrictAlgorithm {
    public:
        const std::string name;
        const std::string desc;
        DistrictAlgorithm(std::string n, std::string d) : name(n), desc(d) {}
        virtual void drawMap(State& s) = 0;
};

extern std::vector<DistrictAlgorithm*> algos;

class MonoDistrictTest : public DistrictAlgorithm {
    public:
        MonoDistrictTest() : DistrictAlgorithm("SingleDistrict", "Test Algorithm that draws a single district") {}
        void drawMap(State& s);
};

class NeighborDistrictTest : public DistrictAlgorithm {
    public:
        NeighborDistrictTest() : DistrictAlgorithm("NeighborTest", "Test Algorithm that draws a single district by neighbors") {}
        void drawMap(State& s);
};