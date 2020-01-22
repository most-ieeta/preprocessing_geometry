#include "ga.hpp"
#include <cmath>

using std::atan2;

namespace proximity_term {
	std::vector<std::vector<double>> geodesic_distances_p1;
	std::vector<std::vector<double>> geodesic_distances_p2;

	double geodesic_distance(Polygon p, size_t p1, size_t p2) {
		if (p1 == p2) return 0;

		size_t min, max;
		min = p1<p2?p1:p2;
		max = p1>p2?p1:p2;

		double direct_distance = 0.0;

		//Direct geodesic distance
		for (size_t i = min; i < max; ++i) {
			direct_distance += SimplePoint::norm(p.normalized_vertices[i], p.normalized_vertices[i+1]);
		}

		double reverse_distance = 0.0;
		for (size_t i = max; i < (p.normalized_vertices.size() - 1); ++i) {
			reverse_distance += SimplePoint::norm(p.normalized_vertices[i], p.normalized_vertices[i+1]);
		}
		reverse_distance += SimplePoint::norm(p.normalized_vertices[p.normalized_vertices.size()-1], p.normalized_vertices[0]);
		for (size_t i = 0; i < min; ++i) {
			reverse_distance += SimplePoint::norm(p.normalized_vertices[i], p.normalized_vertices[i+1]);
		}

		return direct_distance < reverse_distance? direct_distance : reverse_distance;
	}

	void geodesic_initialize(Polygon p1, Polygon p2) {
		//Fills p1
		geodesic_distances_p1 = std::vector<std::vector<double>>(p1.normalized_vertices.size(), std::vector<double>(p1.normalized_vertices.size(), 0));
		for (size_t i = 0; i < (p1.normalized_vertices.size()-1); ++i) {
			for (size_t j = i+1; j < p1.normalized_vertices.size(); ++j) {
				double dist = geodesic_distance(p1, i, j);

				geodesic_distances_p1[i][j] = dist;
				geodesic_distances_p1[j][i] = dist;
			}
		}

		//Fills p2
		geodesic_distances_p2 = std::vector<std::vector<double>>(p2.normalized_vertices.size(), std::vector<double>(p2.normalized_vertices.size(), 0));
		for (size_t i = 0; i < (p2.normalized_vertices.size()-1); ++i) {
			for (size_t j = i+1; j < p2.normalized_vertices.size(); ++j) {
				double dist = geodesic_distance(p2, i, j);

				geodesic_distances_p2[i][j] = dist;
				geodesic_distances_p2[j][i] = dist;
			}
		}

		//TODO: normalize the matrixes. Pairwise_geodesic_dist, L54
	}

	double get_value(Polygon p1, Polygon p2, std::vector<matching> matches) {
		if (geodesic_distances_p1.size() != p1.normalized_vertices.size())
			geodesic_initialize(p1, p2);

		double dist = 0.0;

		for (size_t i = 0; i < matches.size(); ++i) {
			size_t next = (i != matches.size()-1? i+1 : 0);
			
			double diff = std::abs(geodesic_distances_p1[matches[i].first][matches[next].first]
										-geodesic_distances_p2[matches[i].second][matches[next].second]);

			double distance_coef = std::exp(-geodesic_distances_p1[matches[i].first][matches[next].first] *
                                            geodesic_distances_p2[matches[i].second][matches[next].second] 
                                            * 0.1);

			dist += diff*distance_coef;

			size_t previous = (i == 0? matches.size()-1 : i-1);
			diff = std::abs(geodesic_distances_p1[matches[i].first][matches[previous].first]
										-geodesic_distances_p2[matches[i].second][matches[previous].second]);

			distance_coef = std::exp(-geodesic_distances_p1[matches[i].first][matches[previous].first] *
                                        geodesic_distances_p2[matches[i].second][matches[previous].second]
                                        * 0.1);

			dist += diff*distance_coef;
		}

		return dist / (p1.normalized_vertices.size() * (p1.normalized_vertices.size() - 1) / 2);
	}
}

namespace distance_term {
	std::vector<std::vector<double>> point_distances;

	void distance_initialize(Polygon, Polygon) {
		//TODO initialize point term
	}

	std::vector<size_t> get_histogram(Polygon, size_t) {
		return std::vector<size_t>(); //TODO return a polar histogram of point P with P at [0, 0] and centroid at [1, 1]
		//A comparative study using contours and skeletons as shape representations for binary image matching
	}

	double histogram_distance(std::vector<size_t>, std::vector<size_t>) {
		return 0; //TODO histogram distance
		//https://stats.stackexchange.com/questions/7400/how-to-assess-the-similarity-of-two-histograms <- quadratic distance
	}

	double get_value(Polygon, Polygon, std::vector<matching>) {
		return 0; //TODO
	}
}

double eval(Polygon, Polygon, std::vector<matching>) {

	return 0;
}

namespace WEx {
	double get_ISE(const Polygon& p1, const Polygon& p2, const std::vector<matching>& matches) {
		double ise = 0.0;
		std::vector<size_t> simp_1, simp_2;
		for (matching m: matches) {
			simp_1.push_back(m.first);
			simp_2.push_back(m.second);
		}
		ise += Polygon::ise(p1, simp_1);
		ise += Polygon::ise(p2, simp_2);

		return ise/2; //Return average ISE of both polygons
	}

	double get_CR(const Polygon& p1, const Polygon& p2, const std::vector<matching>& matches) {
		double CR1 = static_cast<double>(p1.points.size()) / matches.size();
		double CR2 = static_cast<double>(p2.points.size()) / matches.size();
		return (CR1 + CR2)/2; //Average CR
	}

	double get_Value(const Polygon& p1, const Polygon& p2, std::vector<matching> matches, double x = 2.0) {
		return get_ISE(p1, p2, matches) / std::pow(get_CR(p1, p2, matches), x);
	}
}

namespace Segment_Dist {
	const double magnitude_bin_size = 5, angle_bin_size = 18; //Doubles to avoid warning of divisionby0
	size_t get_bin(const SimplePoint& begin, const SimplePoint& end, const SimplePoint& mid_point) {
		double base_angle = atan2(end.y - begin.y, end.x - begin.x);
		double base_magnitude = SimplePoint::norm(begin, end);

		double target_angle = atan2(mid_point.y - begin.y, mid_point.x - begin.x);
		double target_magnitude = SimplePoint::norm(begin, mid_point);

		double relative_angle = target_angle - base_angle;
															//Target and base angle range: [-pi, +pi].
														   	//Relative angle range: [-2pi+pi/6, 2pi+pi/6]
														   	//We will normalize to [0, 2pi]
		if (relative_angle < 0) relative_angle += 2 * M_PI;

		double relative_magnitude = target_magnitude / base_magnitude;

		//We will use 3 bins for magnitude + 6 bins for angles.
		//Magnitude bin size: 0-1/3   1/3-2/3    2/3-1+
		//Angle bin size: pi/3 -> bins start at -pi/6 so we have a good view around angle 0rad
		size_t magnitude_bin = static_cast<unsigned char>(relative_magnitude / (1/magnitude_bin_size));
		if (magnitude_bin >= magnitude_bin_size) magnitude_bin = magnitude_bin_size-1;

		size_t angle_bin;
		if (relative_angle > (2*M_PI - M_PI/angle_bin_size)) { //Final portion of first bin, since it should be centered on 0.
			angle_bin = 0;
		} else {
			angle_bin = static_cast<unsigned char>((relative_angle + M_PI/angle_bin_size) / (2*M_PI/angle_bin_size));
		}

		return magnitude_bin * magnitude_bin_size + angle_bin;
	}

	double get_histogram_diff(const std::vector<unsigned int>& h1, const std::vector<unsigned int>& h2) {
		//TODO: Histogram different options
		//https://stats.stackexchange.com/questions/7400/how-to-assess-the-similarity-of-two-histograms
		//https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_comparison/histogram_comparison.html
		
		//Normalizes the histograms
		//TODO: all histograms are same size, simplify the loops
		std::vector<double> h1n, h2n;
		{
			unsigned int max_h1 = 0, max_h2 = 0;
			for (unsigned int i: h1)
				max_h1 = (i > max_h1? i : max_h1);
			for (unsigned int i: h2)
				max_h2 = (i > max_h2? i : max_h2);

			for (unsigned int i: h1)
				h1n.push_back(i/static_cast<double>(max_h1));
			for (unsigned int i: h2)
				h2n.push_back(i/static_cast<double>(max_h2));
		}

		//Currently using L1 distance
		double statistic = 0.0;
		for (size_t i = 0; i < h1.size(); ++i) {
			statistic += std::abs(h1n[i] - h2n[i]);
		}


		return statistic;
	}

	std::vector<unsigned int> get_histogram(const Polygon& p, const size_t& point_1, const size_t& point_2) {
		std::vector<unsigned int> histogram(magnitude_bin_size * angle_bin_size, 0);

		if (point_1 < point_2) {
			for (size_t i = point_1 + 1; i <= point_2; ++i) {
				histogram[get_bin(p.points[point_1], p.points[point_2], p.points[i])]++;
			}
		} else {
			for (size_t i = point_1 + 1; i < p.points.size(); ++i) {
				histogram[get_bin(p.points[point_1], p.points[point_2], p.points[i])]++;
			}
			for (size_t i = 0; i <= point_2; ++i) {
				histogram[get_bin(p.points[point_1], p.points[point_2], p.points[i])]++;
			}
		}

		return std::move(histogram);
	}

	double get_val(const Polygon& p1, const Polygon& p2, const std::vector<matching>& matches) {
		double total_diff = 0;
		for (size_t i = 1; i < matches.size(); ++i) {
			std::vector<unsigned int> h1 = get_histogram(p1, matches[i-1].first, matches[i].first);
			std::vector<unsigned int> h2 = get_histogram(p2, matches[i-1].second, matches[i].second);
			total_diff += get_histogram_diff(h1, h2);
		}
		std::vector<unsigned int> h1 = get_histogram(p1, matches[matches.size()-1].first, matches[0].first);
		std::vector<unsigned int> h2 = get_histogram(p2, matches[matches.size()-1].second, matches[0].second);
		total_diff += get_histogram_diff(h1, h2);

		return total_diff / matches.size();
	}
}