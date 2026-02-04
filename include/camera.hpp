#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "hittable.hpp"
#include "material.hpp"
#include <omp.h>
#include <random>
#include <chrono>

class Camera {
private:
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for a sum of pixel samples
  Point3 center;              // Camera center
  Point3 pixel_100_loc;       // Location of pixel 0, 0
  Vec3 pixel_delta_u;         // Offset to pixel to the right
  Vec3 pixel_delta_v;         // Offset to pixel below
  Vec3 u, v, w;               // Camera frame basis vectos;

  Vec3 defocus_disk_u; // Defocus disk horizizontal radius
  Vec3 defocus_disk_v; // Defocus disk vertical radius

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;
    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    // Determine viewport dimesions
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
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
        center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel_100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors
    auto defocus_radius =
        focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  Ray get_ray(int i, int j, std::mt19937 &rng) {
    // Construct a camera ray originating from the origin and directed at
    // randomly sampled points around the pixel location i, j

    auto offset{sample_square(rng)};
    auto pixel_sample{pixel_100_loc + ((i + offset.x()) * pixel_delta_u) +
                      ((j + offset.y()) * pixel_delta_v)};

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample(rng);
    auto ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
  }

  Vec3 sample_square(std::mt19937 &rng) const {
    // Returns the vector to a random point in the [-.5, -.5] - [+.5, +.5] unit
    // space
    static thread_local std::uniform_real_distribution<double> distribution(
        0.0, 1.0);
    return Vec3(distribution(rng) - 0.5, distribution(rng) - 0.5, 0);
  }

  Color ray_color(const Ray &r, int depth, const Hittable &world,
                  std::mt19937 &rng) const {
    if (depth <=
        0) // If we've exeeded the ray bounce limit, no more light is gathered
      return Color(0, 0, 0);

    Hit_Record rec;

    if (world.hit(r, Interval(0.001, INF), rec)) {
      Ray scattered;
      Color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered, rng)) {
        return attenuation * ray_color(scattered, depth - 1, world, rng);
      }
      return Color(0, 0, 0);
    }
    Vec3 unit_direction{unit_vector(r.direction())};
    double a{0.5 * (unit_direction.y() + 1.0)};
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
  }

  Point3 defocus_disk_sample(std::mt19937 &rng) const {
    // Returns random point in rhe camera defocus disk
    auto p{random_in_unit_disk(rng)};
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
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

  double defocus_angle = 0; // Variation angle of rays through each pixel
  double focus_dist =
      10; // Distance from camera lookform point to plane of perfect focus

  void render(const Hittable &world, bool benchmark_mode = false) {
    initialize();

    if (benchmark_mode) {
      // Run benchmark comparing threaded vs single-threaded
      run_benchmark(world);
      return;
    }

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    std::vector<Color> image(image_width * image_height);

    int num_threads = omp_get_max_threads();
    std::clog << "Rendering with " << num_threads << " thread(s)...\n";

    auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; i++) {
        thread_local std::mt19937 rng(std::random_device{}() +
                                      omp_get_thread_num());
        Color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          Ray r{get_ray(i, j, rng)};
          pixel_color += ray_color(r, max_depth, world, rng);
        }
        image[j * image_width + i] = pixel_samples_scale * pixel_color;
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::clog << "Render time: " << duration.count() / 1000.0 << " seconds\n";

    for (int j = 0; j < image_height; j++) {
      if (j % 10 == 0) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << " "
                  << std::flush;
      }
      for (int i = 0; i < image_width; i++) {
        write_color(std::cout, image[j * image_width + i]);
      }
    }
    std::clog << "\rDone.               \n";
  }

private:
  void run_benchmark(const Hittable &world) {
    // Store original settings and use smaller values for quick benchmark
    int orig_width = image_width;
    int orig_samples = samples_per_pixel;

    // Use smaller render for benchmark (adjust as needed)
    image_width = 400;
    samples_per_pixel = 50;
    initialize();

    std::vector<Color> image(image_width * image_height);
    int max_threads = omp_get_max_threads();

    std::clog << "=== BENCHMARK MODE ===\n";
    std::clog << "Image: " << image_width << "x" << image_height << "\n";
    std::clog << "Samples per pixel: " << samples_per_pixel << "\n";
    std::clog << "Max depth: " << max_depth << "\n";
    std::clog << "Available threads: " << max_threads << "\n\n";

    // Test single-threaded
    omp_set_num_threads(1);
    std::clog << "Running single-threaded (1 thread)...\n";
    auto start_single = std::chrono::high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; i++) {
        thread_local std::mt19937 rng(std::random_device{}() +
                                      omp_get_thread_num());
        Color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          Ray r{get_ray(i, j, rng)};
          pixel_color += ray_color(r, max_depth, world, rng);
        }
        image[j * image_width + i] = pixel_samples_scale * pixel_color;
      }
    }

    auto end_single = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(end_single - start_single);
    double time_single = duration_single.count() / 1000.0;

    // Test multi-threaded
    omp_set_num_threads(max_threads);
    std::clog << "Running multi-threaded (" << max_threads << " threads)...\n";
    auto start_multi = std::chrono::high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (int j = 0; j < image_height; j++) {
      for (int i = 0; i < image_width; i++) {
        thread_local std::mt19937 rng(std::random_device{}() +
                                      omp_get_thread_num());
        Color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          Ray r{get_ray(i, j, rng)};
          pixel_color += ray_color(r, max_depth, world, rng);
        }
        image[j * image_width + i] = pixel_samples_scale * pixel_color;
      }
    }

    auto end_multi = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(end_multi - start_multi);
    double time_multi = duration_multi.count() / 1000.0;

    // Report results
    std::clog << "\n=== RESULTS ===\n";
    std::clog << "Single-threaded: " << time_single << " seconds\n";
    std::clog << "Multi-threaded:  " << time_multi << " seconds\n";
    std::clog << "Speedup:         " << (time_single / time_multi) << "x\n";
    std::clog << "Efficiency:      " << (time_single / time_multi / max_threads * 100) << "%\n";

    // Restore original settings
    image_width = orig_width;
    samples_per_pixel = orig_samples;
  }
};

#endif // !CAMERA_HPP
