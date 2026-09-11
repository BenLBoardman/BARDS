#include "algorithm_definition.hpp"

void loadAlgorithms() {
    algos.push_back(new MonoDistrictTest());
    algos.push_back(new SimpleBFS());
    algos.push_back(new SimpleDFS());
}
