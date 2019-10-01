#include "linearsegment.hpp"

double LinearSegment::hausdorff(const LinearSegment& l1, const LinearSegment& l2) {
  double max_dist = -1;

  for (auto p1 : l1.points) {
    double min_dist = std::numeric_limits<double>::max();

    for (auto p2 : l2.points) {
      double dist = norm(p1, p2);
      if (dist < min_dist)
        min_dist = dist;
    }
    if (min_dist > max_dist)
      max_dist = min_dist;
  }

  for (auto p1 : pol2.points) {
    double min_dist = std::numeric_limits<double>::max();

    for (auto p2 : pol1.points) {
      double dist = norm(p1, p2);
      if (dist < min_dist)
        min_dist = dist;
    }
    if (min_dist > max_dist)
      max_dist = min_dist;
  }

  return max_dist;
}
