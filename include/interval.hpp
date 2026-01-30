#ifndef INTERVAL_HPP
#define INTERVAL_HPP

class Interval {
public:
  double min, max;

  Interval() : min(+INF), max(-INF) {} // Default interval is empty
  Interval(double min, double max) : min(min), max(max) {}

  double size() { return max - min; }

  bool contains(double x) { return min <= x && x <= max; }

  bool surrounds(double x) const { return min < x && x < max; }

  static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+INF, -INF);
const Interval Interval::universe = Interval(-INF, +INF);

#endif // !INTERVAL_HPP
