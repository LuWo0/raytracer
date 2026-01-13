#ifndef COLOR_HPP
#define COLOR_HPP

#include "vec3.hpp"
#include <iostream>

using Color = Vec3;

void write_color(std::ostream &out, const Color &pixel_color) {
  double r = pixel_color.x();
  double g = pixel_color.y();
  double b = pixel_color.z();

  // Translate the [0, 1] component values to the byte range [0, 255].
  int rByte = static_cast<int>(255.999 * r);
  int gByte = static_cast<int>(255.999 * g);
  int bByte = static_cast<int>(255.999 * b);

  // Write out the pixel color components.
  out << rByte << " " << gByte << " " << bByte << std::endl;
}

#endif // !COLOR_HPP
