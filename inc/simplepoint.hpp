#ifndef SIMPLE_POINT_
#define SIMPLE_POINT_
#include <cmath>

struct SimplePoint {
	double x, y;
	double linear;
	size_t original_index;
	SimplePoint(double x = 0, double y = 0) : x(x), y(y), linear(0) {};

	static double inline norm(const SimplePoint &p1, const SimplePoint &p2, double dz = 0.0) {
		double dx = p1.x - p2.x;
		double dy = p1.y - p2.y;
		return std::sqrt(dx * dx + dy * dy + dz*dz);
	}

	static SimplePoint get_closest_point(const SimplePoint p, const SimplePoint p1, const SimplePoint p2) {
		double a, b, c;
		if (p1.x != p2.x) { // Not a vertical line
			a = (p1.y - p2.y) / (p1.x - p2.x);
			b = -1;
			c = p1.y - a * p1.x; // General line equation ax + by + c = 0
		} else {               // Vertical line
			a = 1;
			b = 0;
			c = -p1.x;
		}

		SimplePoint ret(0, 0);
		ret.x = (b * (b * p.x - a * p.y) - a * c) / (a * a + b * b);

		ret.y = (a * (-b * p.x + a * p.y) - b * c) / (a * a + b * b);

		//Checks it the point lies on the line segment or on the line outside of the valid segment
			if ((ret.x < p1.x && ret.x < p2.x) || (ret.x > p1.x && ret.x > p2.x)) {
				//If point lies outside, closest point becomes return point.
				if (norm(p, p1) < norm(p, p2)) {
					ret = p1;
				} else {
					ret = p2;
				}
			}

		return ret;
	}
};

#endif // SIMPLE_POINT_
