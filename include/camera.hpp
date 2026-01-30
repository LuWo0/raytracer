#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "hittable.hpp"

class Camera {
private:
  int image_height;     // Rendered image height
  Point3 center;        // Camera center
  Point3 pixel_100_loc; // Location of pixel 0, 0
  Vec3 pixel_delta_u;   // Offset to pixel to the right
  Vec3 pixel_delta_v;   // Offset to pixel below

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = Point3(0, 0, 0);

    // Determine viewport dimesions
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges
    auto viewport_u = Vec3(viewport_width, 0, 0);
    auto viewport_v = Vec3(0, viewport_height, 0);

    // Calculate the horizontal and vertical delta vecotrs from pixel to pixel
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel
    auto viewport_upper_left =
        center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    pixel_100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  Color ray_color(const Ray &r, const Hittable &world) const {
    Hit_Record rec;

    if (world.hit(r, Interval(0, INF), rec)) {
      return 0.5 * (rec.normal + Color(1, 1, 1));
    }
    Vec3 unit_direction{unit_vector(r.direction())};
    double a{0.5 * (unit_direction.y() + 1.0)};
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
  }

public:
  double aspect_ratio = 1.0;
  int image_width = 100;

  void render(const Hittable &world) {
    initialize();

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j{0}; j < image_height; j++) {
      std::clog << "\rScanlines remaining: " << (image_height - j) << " "
                << std::flush;
      for (int i{0}; i < image_width; i++) {
        auto pixel_center{pixel100_loc + (i * pixel_delta_u) +
                          (j * pixel_delta_v)};
        auto ray_direction{pixel_center - center};
        Ray r(center, ray_direction);

        Color pixel_color{ray_color(r, world)};
        write_color(std::cout, pixel_color);
      }
    }
    std::clog << "\rDone.               \n";
  }
};

#endif // !CAMERA_HPP
