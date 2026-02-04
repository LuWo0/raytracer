#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "hittable.hpp"
#include "material.hpp"

class Camera {
private:
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for a sum of pixel samples
  Point3 center;              // Camera center
  Point3 pixel_100_loc;       // Location of pixel 0, 0
  Vec3 pixel_delta_u;         // Offset to pixel to the right
  Vec3 pixel_delta_v;         // Offset to pixel below
  Vec3 u, v, w;               // Camera frame basis vectos;

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;
    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    // Determine viewport dimesions
    auto focal_length = (lookfrom - lookat).length();
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focal_length;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    // Calculate the u, v, w unit basis vectors for the camera coordinate frame
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges
    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    // Calculate the horizontal and vertical delta vecotrs from pixel to pixel
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel
    auto viewport_upper_left =
        center - (focal_length * w) - viewport_u / 2 - viewport_v / 2;
    pixel_100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  Ray get_ray(int i, int j) {
    // Construct a camera ray originating from the origin and directed at
    // randomly sampled points around the pixel location i, j

    auto offset{sample_square()};
    auto pixel_sample{pixel_100_loc + ((i + offset.x()) * pixel_delta_u) +
                      ((j + offset.y()) * pixel_delta_v)};

    auto ray_origin = center;
    auto ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
  }

  Vec3 sample_square() const {
    // Returns the vector to a random point in the [-.5, -.5] - [+.5, +.5] unit
    // space
    return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  Color ray_color(const Ray &r, int depth, const Hittable &world) const {
    if (depth <=
        0) // If we've exeeded the ray bounce limit, no more light is gathered
      return Color(0, 0, 0);

    Hit_Record rec;

    if (world.hit(r, Interval(0.001, INF), rec)) {
      Ray scattered;
      Color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered)) {
        return attenuation * ray_color(scattered, depth - 1, world);
      }
      return Color(0, 0, 0);
    }
    Vec3 unit_direction{unit_vector(r.direction())};
    double a{0.5 * (unit_direction.y() + 1.0)};
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
  }

public:
  double aspect_ratio = 1.0;
  int image_width = 100;
  int samples_per_pixel = 10; // Count of random samples for each pixel
  int max_depth = 10;         // Maximum number of ray bounces into scene

  double vfov = 90;                  // Vertical view angle (Field of view)
  Point3 lookfrom = Point3(0, 0, 0); // Point camera is looking from
  Point3 lookat = Point3(0, 0, -1);  // Point camera is looking at
  Vec3 vup = Vec3(0, 1, 0);          // Camera-relatice "up" direction

  void render(const Hittable &world) {
    initialize();

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j{0}; j < image_height; j++) {
      std::clog << "\rScanlines remaining: " << (image_height - j) << " "
                << std::flush;
      for (int i{0}; i < image_width; i++) {
        Color pixel_color(0, 0, 0);
        for (int sample{0}; sample < samples_per_pixel; sample++) {
          Ray r{get_ray(i, j)};
          pixel_color += ray_color(r, max_depth, world);
        }
        write_color(std::cout, pixel_samples_scale * pixel_color);
      }
    }
    std::clog << "\rDone.               \n";
  }
};

#endif // !CAMERA_HPP
