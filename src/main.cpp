#include "../include/color.hpp"
#include "../include/vec3.hpp"
#include <iostream>
int main() {
  // Image

  int image_width{256};
  int image_height{256};

  // Render
  std::cout << "P3\n"
            << image_width << " " << image_height << "\n255" << std::endl;

  for (int j{0}; j < image_height; j++) {
    std::clog << "\rScanlines remaining: " << (image_height - j) << " "
              << std::flush;

    for (int i{0}; i < image_width; i++) {
      double r = static_cast<double>(i) / (image_width - 1);
      double g = static_cast<double>(j) / (image_height - 1);
      double b = 0.0f;

      Color pixel_color = Color(r, g, b);
      write_color(std::cout, pixel_color);
    }
  }

  std::clog << "\rDone.\t" << std::endl;

  return 0;
}
