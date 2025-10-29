#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <ctime>

// Initialize random seed
inline void initRandom() { srand(static_cast<unsigned>(time(0))); }

// Random float between min and max
inline float randFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

#endif
