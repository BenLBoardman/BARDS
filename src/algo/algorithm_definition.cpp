/**
 * @file algorithm_definition.cpp
 * @brief Implementation of loadAlgorithms() (see algorithm_definition.hpp
 *        for function-level documentation).
 */
#include "algorithm_definition.hpp"

void loadAlgorithms() {
    algos.push_back(new MonoDistrictTest());
    algos.push_back(new SimpleBFS());
    algos.push_back(new SimpleDFS());
    algos.push_back(new MultiBFS());
}