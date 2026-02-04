
#include "../include/raytracing.hpp"

#include "../include/camera.hpp"
#include "../include/hittable.hpp"
#include "../include/hittable_list.hpp"
#include "../include/material.hpp"
#include "../include/sphere.hpp"
#include <memory>

int main() {

  Hittable_List world;

  auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(Point3(0, 1000, 0), 1000, ground_material));

  for (int a{-11}; a < 11; a++) {
    for (int b{-11}; b < 11; b++) {
      auto choose_mat{random_double()};
      Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // Diffuse
          auto albedo{Color::random() * Color::random()};
          sphere_material = make_shared<Lambertian>(albedo);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // Metal
          auto albedo{Color::random(0.5, 1)};
          auto fuzz{random_double(0, 0.5)};
          sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // Glass
          sphere_material = make_shared<Dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1{make_shared<Dielectric>(1.5)};
  world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

  auto material2{make_shared<Lambertian>(Color(0.4, 0.2, 0.1))};
  world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

  auto material3{make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0)};
  world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

  Camera camera;

  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 1200;
  camera.samples_per_pixel = 10;
  camera.max_depth = 50;

  camera.vfov = 20;
  camera.lookfrom = Point3(13, 2, 3);
  camera.lookat = Point3(0, 0, 0);
  camera.vup = Vec3(0, 1, 0);

  camera.defocus_angle = 0.6;
  camera.focus_dist = 10.0;

  camera.render(world);

  return 0;
}
