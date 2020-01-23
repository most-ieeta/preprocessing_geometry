#include "ga.hpp"
#include <random>
#include <algorithm>

namespace {
std::mt19937 gen(1); //Standard mersenne_twister_engine seeded with fixed seed for replicability
};

namespace WEx{
double get_Value(const Polygon& p1, const Polygon& p2, std::vector<matching> matches, double x = 1.0);
};

namespace Segment_Dist {
double get_val(const Polygon& p1, const Polygon& p2, const std::vector<matching>& matches);
};

double eval(Polygon p1, Polygon p2, std::vector<matching> matches, double alpha) {
	return alpha * WEx::get_Value(p1, p2, matches) + (1-alpha) * Segment_Dist::get_val(p1, p2, matches);
}

void local_search(const Polygon& p1, const Polygon& p2, const double alpha, std::vector<matching>& matches) {
	bool improved, outer_improved;
	double value = eval(p1, p2, matches, alpha);

	outer_improved = true;
	while (outer_improved) {
		outer_improved = false;
		for (size_t i = 0; i < matches.size(); ++i) {
			do {
				improved = false;
				std::vector<matching> new_matches = matches;
				new_matches[i].first = (matches[i].first == 0? p1.points.size()-1 : matches[i].first - 1);
				double new_value = eval(p1, p2, new_matches, alpha);
				if (new_value < value) {
					outer_improved = improved = true;
					matches=new_matches;
					value = new_value;
				}

				new_matches[i].first = (matches[i].first == p1.points.size()-1? 0 : matches[i].first + 1);
				new_value = eval(p1, p2, new_matches, alpha);
				if (new_value < value) {
					outer_improved = improved = true;
					matches=new_matches;
					value = new_value;
				}

				new_matches[i].first = matches[i].first;

				new_matches[i].second = (matches[i].second == 0? p2.points.size()-1 : matches[i].second - 1);
				new_value = eval(p1, p2, new_matches, alpha);
				if (new_value < value) {
					outer_improved = improved = true;
					matches=new_matches;
					value = new_value;
				}

				new_matches[i].second = (matches[i].second == p2.points.size()-1? 0 : matches[i].second + 1);
				new_value = eval(p1, p2, new_matches, alpha);
				if (new_value < value) {
					outer_improved = improved = true;
					matches=new_matches;
					value = new_value;
				}
			} while (improved);
		}
	}
}


std::vector<matching> GRASP(Polygon p1, Polygon p2, double alpha) {
	//Builds initial matching randomly
	size_t pt1, pt2;
	std::vector<matching> matches;
	{
		std::uniform_int_distribution<size_t> dis1(0, p1.points.size()-1);
		std::uniform_int_distribution<size_t> dis2(0, p2.points.size()-1);
		pt1 = dis1(gen);
		pt2 = dis2(gen);
		matches.push_back(std::make_pair(pt1, pt2));

		while ((pt1 = dis1(gen)) == matches[0].first) { };
		while ((pt2 = dis2(gen)) == matches[0].second) { };

		matches.push_back(std::make_pair(pt1, pt2));

		//Wiggle first pair
		bool improved;
		double value = eval(p1, p2, matches, alpha);
		do {
			improved = false;
			std::vector<matching> new_matches = matches;
			new_matches[0].first = (matches[0].first == 0? p1.points.size()-1 : matches[0].first - 1);
			double new_value = eval(p1, p2, new_matches, alpha);
			if (new_value < value) {
				improved = true;
				matches=new_matches;
				value = new_value;
			}

			new_matches[0].first = (matches[0].first == p1.points.size()-1? 0 : matches[0].first + 1);
			new_value = eval(p1, p2, new_matches, alpha);
			if (new_value < value) {
				improved = true;
				matches=new_matches;
				value = new_value;
			}

			new_matches[0].first = matches[0].first;

			new_matches[0].second = (matches[0].second == 0? p2.points.size()-1 : matches[0].second - 1);
			new_value = eval(p1, p2, new_matches, alpha);
			if (new_value < value) {
				improved = true;
				matches=new_matches;
				value = new_value;
			}

			new_matches[0].second = (matches[0].second == p2.points.size()-1? 0 : matches[0].second + 1);
			new_value = eval(p1, p2, new_matches, alpha);
			if (new_value < value) {
				improved = true;
				matches=new_matches;
				value = new_value;
			}
		} while (improved);
	}

	double value = eval(p1, p2, matches, alpha);
	double improve = 0;
	do {
		//Future: create RCL
		improve = 1;
		for (unsigned int n = 0; n < 50; ++n) {
			std::vector<matching> new_matches = matches;
			matching m = get_valid_point(new_matches, p1.points.size(), p2.points.size());
			if (m.first == p1.points.size()) {
				continue;
			}
			new_matches.push_back(m);
			std::sort(new_matches.begin(), new_matches.end(), [](matching m1, matching m2) { return m1.first < m2.first; });
			double new_value = eval(p1, p2, new_matches, alpha);

			if (new_value < value) {
				improve = new_value/value;
				value = new_value;
				matches = new_matches;
				break;
			}
		}
		//std::cout << "Advanced one. Value: " << value << ". n=" << matches.size() << std::endl;
	} while (improve <= 0.99);

	local_search(p1, p2, alpha, matches);
	//std::cout << "Finished a grasp. Value: " << value << ". n=" << matches.size() << std::endl;
	//std::cout << value << " " << WEx::get_Value(p1, p2, matches) << " " << Segment_Dist::get_val(p1, p2, matches) << "\n";
	return std::move(matches);
}