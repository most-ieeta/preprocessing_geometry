#include "polygon.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <string>

/** Returns the distance between the points. Provided for deprecated
 * OpenCV::Point compatibility
 * */
double norm(const SimplePoint &p1, const SimplePoint &p2) {
  double dx = p1.x - p2.x;
  double dy = p1.y - p2.y;
  return std::sqrt(dx * dx + dy * dy);
}

/** Reads a file stream of type specified by FileType
 * */
Polygon::Polygon(std::istream &input_file, FileType ftype) {
  if (ftype == FileType::FILE_POF) {
    double x, y;
    while (input_file >> x >> y) {
      points.emplace_back(x, y);
    }
  } else if (ftype == FileType::FILE_WKT) { // Reads WKT
    GEOSContextHandle_t geos;
    geos = GEOS_init_r();

    GEOSWKTReader *r;
    r = GEOSWKTReader_create_r(geos);

    GEOSGeom poly;
    std::string s;
    std::getline(input_file, s);
    poly = GEOSWKTReader_read_r(
        geos, r, s.c_str()); // TODO: check 1 line. check if polygon
    const GEOSGeometry *ring = GEOSGetExteriorRing_r(geos, poly);

    const GEOSCoordSequence *coords = GEOSGeom_getCoordSeq_r(geos, ring);

    uint size;
    GEOSCoordSeq_getSize_r(geos, coords, &size);

    for (uint i = 0; i < (size - 1);
         ++i) { // WKT polygon must have final point the same as initial; our
                // representation assumes that
      double x, y;
      GEOSCoordSeq_getX_r(geos, coords, i, &x);
      GEOSCoordSeq_getY_r(geos, coords, i, &y);
      points.emplace_back(x, y);
    }

    GEOSGeom_destroy_r(geos, poly); // Destroy the polygon. External ring and
                                    // coordseq are owned by polygom Geom object
    GEOSWKTReader_destroy_r(geos, r); // Free the reader
    GEOS_finish_r(geos);              // Finish the context
  }

	//Calculates linear coordinates
	double accum = 0.0;
	double perim = 0.0;
	for (size_t i = 0; i < (points.size() - 1); ++i) {
		perim += SimplePoint::norm(points[i], points[i+1]);
	}
	perim += SimplePoint::norm(points[0], points[points.size()-1]);

	for (size_t i = 0; i < points.size(); ++i) {
		if (i != 0)
			accum += SimplePoint::norm(points[i], points[i-1]);
		points[i].linear = accum/perim;
	}
}

/** Saves to specific stream with format specified by FileType
 * */
void Polygon::save(std::ostream &output_file, FileType ftype) const {
  if (ftype == FileType::FILE_POF) {
    for (auto p : points) {
      output_file << p.x << " " << p.y << "\n";
    }
  } else if (ftype == FileType::FILE_WKT) {
    GEOSContextHandle_t geos;
    geos = GEOS_init_r();

    GEOSCoordSequence *cs = GEOSCoordSeq_create_r(geos, points.size() + 1,
                                                  2); // 2 dimensional shapes

    for (size_t i = 0; i < points.size(); ++i) {
      GEOSCoordSeq_setX_r(geos, cs, i, points[i].x);
      GEOSCoordSeq_setY_r(geos, cs, i, points[i].y);
    }
    GEOSCoordSeq_setX_r(geos, cs, points.size(), points[0].x);
    GEOSCoordSeq_setY_r(geos, cs, points.size(), points[0].y);

    GEOSGeom lring = GEOSGeom_createLinearRing_r(
        geos, cs); // Creates the linear ring - I lose ownership of CS here
    GEOSGeom poly = GEOSGeom_createPolygon_r(
        geos, lring, NULL,
        0); // Creates the polygon. I lose ownership of lring here.

    GEOSWKTWriter *w = GEOSWKTWriter_create_r(geos);
    GEOSWKTWriter_setRoundingPrecision_r(geos, w, 0);
    GEOSWKTWriter_setOutputDimension_r(geos, w, 2);

    char *line = GEOSWKTWriter_write_r(geos, w, poly);

    output_file << line;

    free(line);                       // Destroys line
    GEOSWKTWriter_destroy_r(geos, w); // Destroys WKTWriter

    // cs is owned by GEOSGeom lring.
    // lring is owned by GEOSGeom poly
    GEOSGeom_destroy(poly); // Destroy poly
    GEOS_finish_r(geos);    // Destroy handler
  }
}

/** Calculates Hausdorff distance between two Polygons
 * */
double Polygon::hausdorff(const Polygon &pol1, const Polygon &pol2) {
  double max_dist = -1;

  for (auto p1 : pol1.points) {
    double min_dist = std::numeric_limits<double>::max();

    for (auto p2 : pol2.points) {
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

/** Returns GEOS Polygon. IMPORTANT: YOU MUST DESTROY USING GEOSGeom_destroy!!!!
*/
GEOSGeom Polygon::get_GEOSGeom(GEOSContextHandle_t geos) const {
    GEOSCoordSequence *cs = GEOSCoordSeq_create_r(geos, points.size() + 1,
                                                  2); // 2 dimensional shapes

    for (size_t i = 0; i < points.size(); ++i) {
      GEOSCoordSeq_setX_r(geos, cs, i, points[i].x);
      GEOSCoordSeq_setY_r(geos, cs, i, points[i].y);
    }
    GEOSCoordSeq_setX_r(geos, cs, points.size(), points[0].x);
    GEOSCoordSeq_setY_r(geos, cs, points.size(), points[0].y);

    GEOSGeom lring = GEOSGeom_createLinearRing_r(
        geos, cs); // Creates the linear ring - I lose ownership of CS here
    GEOSGeom poly = GEOSGeom_createPolygon_r(
        geos, lring, NULL,
        0); // Creates the polygon. I lose ownership of lring here.

    // cs is owned by GEOSGeom lring.
    // lring is owned by GEOSGeom poly
		return poly;
}


/** Calculates Jaccard Index between two polygons
 * */
double Polygon::jaccard(const Polygon& pol1, const Polygon& pol2) {
	GEOSContextHandle_t geos;
	geos = GEOS_init_r();

	GEOSGeom poly1 = pol1.get_GEOSGeom(geos);
	GEOSGeom poly2 = pol2.get_GEOSGeom(geos);

	GEOSGeom intersection_geometry = GEOSIntersection_r(geos, poly1, poly2);
	GEOSGeom union_geometry = GEOSUnion_r(geos, poly1, poly2);

	double area_intersection;
	GEOSArea_r(geos, intersection_geometry, &area_intersection);


	double area_union;
	GEOSArea_r(geos, union_geometry, &area_union);

	GEOSGeom_destroy(poly1);
	GEOSGeom_destroy(poly2);
	GEOSGeom_destroy(union_geometry);
	GEOSGeom_destroy(intersection_geometry);

	GEOS_finish_r(geos);    // Destroy handler

	return area_intersection/area_union;
}

void Polygon::print(GEOSContextHandle_t geos, char* prefix) {
	GEOSWKTWriter *w = GEOSWKTWriter_create_r(geos);
	GEOSWKTWriter_setRoundingPrecision_r(geos, w, 2);
	GEOSWKTWriter_setOutputDimension_r(geos, w, 2);

	GEOSGeom poly = get_GEOSGeom(geos);
	char *line = GEOSWKTWriter_write_r(geos, w, poly);

	if (prefix != nullptr)
		std::cout << prefix;
	std::cout << line << std::endl;

	free(line);                       // Destroys line
	GEOSWKTWriter_destroy_r(geos, w); // Destroys WKTWriter
	GEOSGeom_destroy_r(geos, poly);
}

/** Calculates Chamfer distance between two polygons
 * */
double Polygon::chamfer(const Polygon &pol1, const Polygon &pol2) {
	double c12 = 0, c21 = 0; // Directed chamfer distances

	for (auto p1 : pol1.points) {
		double min_dist = std::numeric_limits<double>::max();
		for (auto p2 : pol2.points) {
			double dist = norm(p1, p2);

			if (dist < min_dist)
				min_dist = dist;
		}
		c12 += min_dist;
	}
	c12 /= pol1.points.size();

	for (auto p2 : pol2.points) {
		double min_dist = std::numeric_limits<double>::max();
		for (auto p1 : pol1.points) {
			double dist = norm(p1, p2);

			if (dist < min_dist)
				min_dist = dist;
		}
		c21 += min_dist;
	}
	c21 /= pol2.points.size();

	return (c12 + c21) / 2;
}

/** Gets point closest to P on line defined by p1 and p2
 * */
SimplePoint get_closest_point(const SimplePoint p, const SimplePoint p1,
		const SimplePoint p2) {
	double a, b, c;
	if (p1.x != p2.x) { // Not a vertical line
		a = (p1.y - p2.y) / (p1.x - p2.x);
		b = -1;
		c = p1.y - a * p2.x; // General line equation ax + by + c = 0
	} else {               // Vertical line
		a = 1;
		b = 0;
		c = -p1.x;
	}

	SimplePoint ret(0, 0);
	ret.x = (b * (b * p.x - a * p.y) - a * c) / (a * a + b * b);

	ret.y = (a * (-b * p.x + a * p.y) - b * c) / (a * a + b * b);

	return ret;
}

/** Calculates polis distance between two polygons
 * */
double Polygon::polis(const Polygon &pol1, const Polygon &pol2) {
	double d_12 = 0, d_21 = 0; // Directed distances

	// Block to calc directed distance between 1 and 2
	{
		for (size_t i1 = 0; i1 < pol1.points.size(); ++i1) {
			double min_dist = std::numeric_limits<double>::max();

			// Checks min dist between all points and line segments
			for (size_t i2 = 0; i2 < (pol2.points.size() - 1); ++i2) {
				double d_p1, d_p2; // Distance to both vertexes
				double d_line;     // Distance to the point closes in line
				SimplePoint p =
					get_closest_point(pol1.points[i1], pol2.points[i2],
							pol2.points[i2 + 1]); // Point in line closest to
				// the evaluated point

				d_p1 = norm(pol1.points[i1], pol2.points[i2]);
				d_p2 = norm(pol1.points[i1], pol2.points[i2 + 1]);

				if ((p.x < pol2.points[i2].x && p.x < pol2.points[i2 + 1].x) ||
						(p.x > pol2.points[i2].x && p.x > pol2.points[i2 + 1].x)) {
					d_line = std::numeric_limits<double>::max();
				} else {
					d_line = norm(pol1.points[i1], p);
				}

				if (d_p1 < min_dist)
					min_dist = d_p1;

				if (d_p2 < min_dist)
					min_dist = d_p2;

				if (d_line < min_dist)
					min_dist = d_line;
			}

			d_12 += min_dist;
		}

		d_12 /= pol1.points.size();
	}

	// Block to calc directed distance between 2 and 1
	{
		for (size_t i2 = 0; i2 < pol2.points.size(); ++i2) {
			double min_dist = std::numeric_limits<double>::max();

			// Checks min dist between all points and line segments
			for (size_t i1 = 0; i1 < (pol1.points.size() - 1); ++i1) {
				double d_p1, d_p2; // Distance to both vertexes
				double d_line;     // Distance to the point closes in line
				SimplePoint p =
					get_closest_point(pol2.points[i2], pol1.points[i1],
							pol1.points[i1 + 1]); // Point in line closest to
				// the evaluated point

				d_p1 = norm(pol2.points[i2], pol1.points[i1]);
				d_p2 = norm(pol2.points[i2], pol1.points[i1 + 1]);

				if ((p.x < pol1.points[i1].x && p.x < pol1.points[i1 + 1].x) ||
						(p.x > pol1.points[i1].x && p.x > pol1.points[i1 + 1].x)) {
					d_line = std::numeric_limits<double>::max();
				} else {
					d_line = norm(pol2.points[i2], p);
				}

				if (d_p1 < min_dist)
					min_dist = d_p1;

				if (d_p2 < min_dist)
					min_dist = d_p2;

				if (d_line < min_dist)
					min_dist = d_line;
			}

			d_21 += min_dist;
		}

		d_21 /= pol2.points.size();
	}
	return (d_12 + d_21) / 2;
}

