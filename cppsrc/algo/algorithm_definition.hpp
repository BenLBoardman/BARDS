#include "../classdefs.hpp"
#include "../geometry.hpp"
#include "../dataset.hpp"

/**
 * An extendible base to implement an algorithm to draw a district. Includes an empty constructor and an entry point.
 */
class DistrictAlgorithm {
    public:
        DistrictAlgorithm(){}
        virtual void drawMap(State& s) = 0;
};

class MonoDistrictTest : public DistrictAlgorithm {
    public:
        void drawMap(State& s);
};

class NeighborDistrictTest : public DistrictAlgorithm {
    public:
        void drawMap(State& s);
};