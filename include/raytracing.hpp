#ifndef RAYTRACING_HPP
#define RAYTRACING_HPP

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// C++ std Usings
using std::make_shared;
using std::shared_ptr;

// Constants
const double INF{std::numeric_limits<double>::infinity()};
const double PI{3.1415926535897932385};

// Utility Functions
inline double degrees_to_radians(double degrees) {
  return degrees * PI / 180.0;
}

// Common Headers

#include "color.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#endif // !RAYTRACING_HPP
