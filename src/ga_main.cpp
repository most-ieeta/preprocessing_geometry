#include <iostream>
#include <fstream>
#include "ga.hpp"
#include "polygon.hpp"

namespace proximity_term { 
double geodesic_distance(Polygon p, size_t p1, size_t p2);
};

using proximity_term::geodesic_distance;

int main(int, char**) {
	std::fstream fs("sq1.pof");
	Polygon p1(fs, Polygon::FileType::FILE_POF);

	for (size_t i = 0; i < p1.points.size(); ++i) {
		for (size_t j = i+1; j < p1.points.size(); ++j) {
			std::cout << "Geodesic p1(" << i << ", " << j << "): " << geodesic_distance(p1, i, j) << "\n";
		}
	}

	std::cout << "\n\n\n\n";
	fs = std::fstream("sq2.pof");
	Polygon p2(fs, Polygon::FileType::FILE_POF);

	for (size_t i = 0; i < p2.points.size(); ++i) {
		for (size_t j = i+1; j < p2.points.size(); ++j) {
			std::cout << "Geodesic p2(" << i << ", " << j << "): " << geodesic_distance(p2, i, j) << "\n";
		}
	}


	std::cout << "\n\n\n";

	proximity_term::geodesic_initialize(p1, p2);

	std::vector<matching> v;
	v.push_back({0, 0});
	v.push_back({3, 3});

	std::cout << proximity_term::get_value(p1, p2, v) << std::endl;

	return 0;
}
