#ifndef SIMPLIFIER_HPP_
#define SIMPLIFIER_HPP_
#include "polygon.hpp"
#include "simplepoint.hpp"
#include <limits>

namespace Simplifier {

	void tetrahedral_until_n_points(std::vector<Polygon> polys, float red_percentage); 
	void visvalingam_with_time(std::vector<Polygon>& polys, float red_percentage, float time_value);
	
	void visvalingam_until_n(Polygon& poly, const float& red_percentage);

	void visvalingam_with_corr(std::vector<Polygon>& polys, 
			                       float red_percentage, 
														 float area_preference 
														  );
}; // namespace Simplifier

#endif //SIMPLIFIER_HPP_
