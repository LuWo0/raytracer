#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hittable.hpp"

class Sphere : public Hittable {
private:
  Point3 center;
  double radius;

public:
  Sphere(const Point3 &center, double radius)
      : center(center), radius(std::fmax(0, radius)) {}

  bool hit(const Ray &r, Interval ray_t, Hit_Record &rec) const override {
    Vec3 oc{center - r.origin()};
    auto a{r.direction().length_squared()};
    auto h{dot(r.direction(), oc)};
    auto c{oc.length_squared() - radius * radius};

    auto discriminant{h * h - a * c};
    if (discriminant < 0) {
      return false;
    }

    auto sqrtd{std::sqrt(discriminant)};

    // Find the nearest root that lies in the acceptable range
    auto root{(h - sqrtd) / a};
    if (!ray_t.surrounds(root)) {
      root = (h + sqrtd) / a;
      if (!ray_t.surrounds(root)) {
        return false;
      }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    return true;
  }
};

#endif // !SHERE_HPP
