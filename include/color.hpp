#ifndef COLOR_HPP
#define COLOR_HPP

#include "interval.hpp"
#include "vec3.hpp"

using Color = Vec3;

void write_color(std::ostream &out, const Color &pixel_color) {
  double r{pixel_color.x()};
  double g{pixel_color.y()};
  double b{pixel_color.z()};

  // Translate the [0, 1] component values to the byte range [0, 255].
  static const Interval intensity(0.000, 0.999);
  int rByte = static_cast<int>(256 * intensity.clamp(r));
  int gByte = static_cast<int>(256 * intensity.clamp(g));
  int bByte = static_cast<int>(256 * intensity.clamp(b));

  // Write out the pixel color components.
  out << rByte << " " << gByte << " " << bByte << std::endl;
}

#endif // !COLOR_HPP
