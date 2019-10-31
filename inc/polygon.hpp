#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <vector>
#include <iostream>
#include <geos_c.h>
#include "simplepoint.hpp"

class Polygon {
    public:
        std::vector<SimplePoint> points;
        std::vector<SimplePoint> normalized_vertices;
				double perimeter;

        enum class FileType {
            FILE_POF,
            FILE_WKT
        };

        Polygon(std::istream& input_file, FileType ftype = FileType::FILE_POF);
        Polygon() {};
        

        void save(std::ostream& output_file, FileType ftype = FileType::FILE_POF) const;

        void normalize_coords();

        //Static methods
        static double hausdorff(const Polygon& pol1, const Polygon& pol2);
        static double chamfer(const Polygon& pol1, const Polygon& pol2);
        static double polis(const Polygon& pol1, const Polygon& pol2);
				static double jaccard(const Polygon& pol1, const Polygon& pol2);

			private:
				GEOSGeom get_GEOSGeom(GEOSContextHandle_t geos) const;
				void print(GEOSContextHandle_t geos, char* prefix = nullptr);
};

#endif //POLYGON_HPP
