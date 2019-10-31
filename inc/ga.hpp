#ifndef GA_H_
#define GA_H_
#include <utility>
#include "polygon.hpp"
#include <vector>

typedef std::pair<size_t, size_t> matching;

double eval(Polygon p, Polygon q, std::vector<matching> matches);

//TODO DEBUG
namespace proximity_term {
	void geodesic_initialize(Polygon p1, Polygon p2);
	double get_value(Polygon p1, Polygon p2, std::vector<matching> matches);
}

#endif//GA_H_
