#ifndef VEC3_HPP
#define VEC3_HPP

#include <random>

class Vec3 {
public:
  double e[3];

  Vec3() : e{0, 0, 0} {}
  Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }

  Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
  double operator[](int i) const { return e[i]; }
  double &operator[](int i) { return e[i]; }

  Vec3 &operator+=(const Vec3 &v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  Vec3 &operator*=(double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  Vec3 &operator/=(double t) { return *this *= (1 / t); }

  double length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  double length() const { return std::sqrt(length_squared()); }

  static Vec3 random(std::mt19937 &rng) {
    static thread_local std::uniform_real_distribution<double> distribution(
        0.0, 1.0);
    return Vec3(distribution(rng), distribution(rng), distribution(rng));
  }

  static Vec3 random() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return Vec3(distribution(generator), distribution(generator),
                distribution(generator));
  }

  static Vec3 random(double min, double max) {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    double range = max - min;
    return Vec3(min + range * distribution(generator),
                min + range * distribution(generator),
                min + range * distribution(generator));
  }

  static Vec3 random(double min, double max, std::mt19937 &rng) {
    static thread_local std::uniform_real_distribution<double> distribution(
        0.0, 1.0);
    double range = max - min;
    return Vec3(min + range * distribution(rng),
                min + range * distribution(rng),
                min + range * distribution(rng));
  }

  bool near_zero() const {
    // Return true if the vector is close to zero in all dimension
    auto s = 1e-8;
    return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) &&
           (std::fabs(e[2]) < s);
  }
};

// Point3 is just an alias for Vec3, but useful for geometric clarity in the
// code.
using Point3 = Vec3;

// Vector Utility Functions

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
  return out << v.e[0] << v.e[1] << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
  return Vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline Vec3 operator*(const Vec3 &v, double t) { return t * v; }

inline Vec3 operator/(const Vec3 &v, double t) { return (1 / t) * v; }

inline double dot(const Vec3 &u, const Vec3 &v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(const Vec3 &v) { return v / v.length(); }

inline Vec3 random_in_unit_disk(std::mt19937 &rng) {
  static thread_local std::uniform_real_distribution<double> distribution(-1.0,
                                                                          1.0);
  while (true) {
    auto p = Vec3(distribution(rng), distribution(rng), 0);
    if (p.length_squared() < 1)
      return p;
  }
}
inline Vec3 random_unit_vector(std::mt19937 &rng) {
  static thread_local std::uniform_real_distribution<double> distribution(-1.0,
                                                                          1.0);
  while (true) {
    auto p = Vec3(distribution(rng), distribution(rng), distribution(rng));
    auto lensq = p.length_squared();
    if (1e-160 < lensq && lensq <= 1) {
      return p / sqrt(lensq);
    }
  }
}

inline Vec3 random_on_hemisphere(const Vec3 &normal, std::mt19937 &rng) {
  Vec3 on_unit_sphere{random_unit_vector(rng)};
  if (dot(on_unit_sphere, normal) >
      0.0) { // In the same hemisphere as the normal
    return on_unit_sphere;
  }
  return -on_unit_sphere;
}

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) {
  return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3 &uv, const Vec3 &n, double etal_over_etat) {
  auto cos_theta{std::fmin(dot(-uv, n), 1.0)};
  Vec3 r_out_perp{etal_over_etat * (uv + cos_theta * n)};
  Vec3 r_out_parallel{-std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) *
                      n};
  return r_out_perp + r_out_parallel;
}

#endif // !VEC3_HPP
