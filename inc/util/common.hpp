#pragma once

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

// tutorial has an 'using std' for shared pointers and maked shared functions, but its not the best practice to have that in a common for naming clarity reasons
// I also stripped the header of common headers for the sake of being explicit; I might need to add stuff later to this, but right now its pretty bare bones

// Constant values
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Util functions
inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
	return min + (max - min) * random_double(); // returns random number in range of min and max
}