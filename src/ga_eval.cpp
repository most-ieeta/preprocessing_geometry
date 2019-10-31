#include "ga.hpp"

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
			/* TODO remove */std::cout << "First distance  (point to next): " << diff << std::endl;
			double distance_coef = std::exp(-geodesic_distances_p1[matches[i].first][matches[next].first] *
                                            geodesic_distances_p2[matches[i].second][matches[next].second] 
                                            * 0.1);

			dist += diff*distance_coef;

			size_t previous = (i == 0? matches.size()-1 : i-1);
			diff = std::abs(geodesic_distances_p1[matches[i].first][matches[previous].first]
										-geodesic_distances_p2[matches[i].second][matches[previous].second]);
			/* TODO remove */std::cout << "First distance  (point to previous): " << diff << std::endl;
			distance_coef = std::exp(-geodesic_distances_p1[matches[i].first][matches[previous].first] *
                                        geodesic_distances_p2[matches[i].second][matches[previous].second]
                                        * 0.1);

			dist += diff*distance_coef;
		}

		return dist / (p1.normalized_vertices.size() * (p1.normalized_vertices.size() - 1) / 2);
	}
}
double eval(Polygon, Polygon, std::vector<matching>) {

	return 0;
}
