#ifndef LINSEG_HPP
#define LINSEG_HPP

#include <vector>
#include "simplepoint.hpp"

class LinearSegment {
	public: 
		std::vector<SimplePoint> points;

		LinearSegment() {};
		//TODO: load/save?

		static double hausdorff(const LinearSegment& l1, const LinearSegment& l2);
}
#endif //LINSEG_HPP
