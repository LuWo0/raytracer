
#include "../include/raytracing.hpp"

#include "../include/camera.hpp"
#include "../include/hittable.hpp"
#include "../include/hittable_list.hpp"
#include "../include/sphere.hpp"

int main() {

  Hittable_List world;
  world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
  world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

  Camera camera;
  camera.aspect_ratio = 16.0 / 9.0;
  camera.image_width = 400;
  camera.samples_per_pixel = 100;

  camera.render(world);

  return 0;
}
