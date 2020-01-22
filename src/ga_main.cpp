#include <iostream>
#include <fstream>
#include <algorithm>


#include "ga.hpp"
#include "polygon.hpp"


//#define TEST_SEG_DIST
//#define TEST_MATCHING
#define TEST_GRASP

namespace WEx{
double get_Value(const Polygon& p1, const Polygon& p2, std::vector<matching> matches, double x = 2.0);
};

namespace Segment_Dist {
double get_val(const Polygon& p1, const Polygon& p2, const std::vector<matching>& matches);
};

#ifdef TEST_SEG_DIST
namespace Segment_Dist {
	double get_val(Polygon p1, Polygon p2, std::vector<matching> matches);
}
#endif

#ifdef TEST_MATCHING
matching get_valid_point(std::vector<matching> matches, size_t max_size1, size_t max_size2);
size_t get_valid_point(std::vector<matching> matches, size_t max_size);
#endif

#ifdef TEST_GRASP
#define ALPHA 1
std::vector<matching> GRASP(Polygon p1, Polygon p2, double alpha = ALPHA);
double eval(Polygon p1, Polygon p2, std::vector<matching> matches, double alpha);
void local_search(const Polygon& p1, const Polygon& p2, const double alpha, std::vector<matching>& matches);
#endif

int main(int, char**) {
	/*std::fstream fs("arb01.pof");
	Polygon pol1(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("arb01s.pof");
	Polygon pol2(fs, Polygon::FileType::FILE_POF);*/
#ifdef TEST_SEG_DIST
	std::fstream fs("sq1.pof");
	Polygon p1(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("sq2.pof");
	Polygon p2(fs, Polygon::FileType::FILE_POF);

	/*Seg_Dist::get_bin(SimplePoint(0, 0), SimplePoint(1,0), SimplePoint(0.5, 0.5));
	Seg_Dist::get_bWExin(SimplePoint(0, 0), SimplePoint(1,0), SimplePoint(0, 0.5));
	Seg_Dist::get_bin(SimplePoint(0, 0), SimplePoint(1,0), SimplePoint(0.1, 0.1));*/

	std::vector<matching> v;
	v.emplace_back(std::make_pair<size_t, size_t>(0, 0));
	v.emplace_back(std::make_pair<size_t, size_t>(3, 3));
	v.emplace_back(std::make_pair<size_t, size_t>(5, 4));
	std::cout << "\nAvgHistDistance: " << Segment_Dist::get_val(p1, p2, v) << "\n";

	std::cout << "WE2 Distance: " << WEx::get_Value(p1, p2, v) << "\n";
#endif

#ifdef TEST_MATCHING
	std::fstream fs("sq1.pof");
	Polygon pol1(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("sq2.pof");
	Polygon pol2(fs, Polygon::FileType::FILE_POF);

	std::vector<matching> v3;
	v3.push_back(std::make_pair<size_t, size_t>(0,0));
	v3.push_back(std::make_pair<size_t, size_t>(8,8));
	v3.push_back(std::make_pair<size_t, size_t>(10,10));
	v3.push_back(std::make_pair<size_t, size_t>(12,12));
	v3.push_back(std::make_pair<size_t, size_t>(14,14));
	v3.push_back(std::make_pair<size_t, size_t>(16, 16));
	v3.push_back(std::make_pair<size_t, size_t>(24, 24));
	double x = WEx::get_Value(pol1, pol2, v3, 2);
	std::cout << __func__ << " WEx(2): " << x << std::endl;
	
	x = Segment_Dist::get_val(pol1, pol2, v3);
	std::cout << __func__ << " WEx(3): " << x << std::endl;


#endif

#ifdef TEST_GRASP
	std::fstream fs("arb01.pof");
	Polygon pol1(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("arb02.pof");
	Polygon pol2(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("car04.pof");
	Polygon pol3(fs, Polygon::FileType::FILE_POF);

	/*fs = std::fstream("match.csv");
	std::vector<matching> matches; // = GRASP(pol1, pol2);

	size_t x, y;
	while (fs >> x >> y) {
      matches.emplace_back(std::make_pair(x, y));
    }

	local_search(pol1, pol2, 1, matches);*/
	
	std::vector<matching> best = GRASP(pol1, pol2, 1);

	double best_value = eval(pol1, pol2, best, ALPHA);
	for (size_t i = 0; i < 100; ++i) {
		std::vector<matching> matches = GRASP(pol1, pol2);
		double value = eval(pol1, pol2, matches, ALPHA);
		if (value < best_value) {
			best = matches;
			best_value = value;
		}
	}

	std::cout << "Arb01 and Arb02: " << best_value << "(" << best.size() << ")\n";
	std::ofstream ofs("out.csv");
	for (matching m: best) {
		ofs << pol1.points[m.first].x << " " << pol1.points[m.first].y << " " <<
		       pol2.points[m.second].x << " " << pol2.points[m.second].y << " " <<
			   m.first << " " << m.second <<  "\n"; 
	}

	return 0;
	
	
	//Writes p1
	char c = 'A';
	std::ofstream out1("arb01s.pof");
	std::ofstream out2("arb02s.pof");
	for (matching m: best) {
		out1 << pol1.points[m.first].x << " "  << pol1.points[m.first].y << " " << c << "\n";
		out2 << pol2.points[m.second].x << " "  << pol2.points[m.second].y << " " << c << "\n";
		c++;
	}

	best = GRASP(pol1, pol3);
	best_value = eval(pol1, pol3, best, ALPHA);
	for (size_t i = 0; i < 100; ++i) {
		std::vector<matching> matches = GRASP(pol1, pol3);
		double value = eval(pol1, pol3, matches, ALPHA);
		if (value < best_value) {
			best = matches;
			best_value = value;
		}
	}
	std::cout << "Arb01 and Car04: " << best_value << "(" << best.size() << ")\n";

	best = GRASP(pol2, pol3);
	best_value = eval(pol2, pol3, best, ALPHA);
	for (size_t i = 0; i < 100; ++i) {
		std::vector<matching> matches = GRASP(pol1, pol3);
		double value = eval(pol2, pol3, matches, ALPHA);
		if (value < best_value) {
			best = matches;
			best_value = value;
		}
	}
	std::cout << "Arb02 and Car04: " << best_value << "(" << best.size() << ")\n";

	fs = std::fstream("sq1.pof");
	Polygon sq1(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("sq2.pof");
	Polygon sq2(fs, Polygon::FileType::FILE_POF);
	best = GRASP(sq1, sq2);
	best_value = eval(sq1, sq2, best, ALPHA);
	for (size_t i = 0; i < 100; ++i) {
		std::vector<matching> matches = GRASP(sq1, sq2);
		double value = eval(sq1, sq2, matches, ALPHA);
		if (value < best_value) {
			best = matches;
			best_value = value;
		}
	}
	std::cout << "sq1 and sq2: " << best_value << "(" << best.size() << ")\n";

#endif

	/*std::fstream fs("sq1.pof");
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

	std::cout << proximity_term::get_value(p1, p2, v) << std::endl; */

	return 0;
}
