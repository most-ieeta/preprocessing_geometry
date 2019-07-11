# preprocessing_geometry
Package of preprocessing geometries for MoST project.

Dependecies:
 - GEOS library (C interface) 3.6+

Main features implemented:
 1. Polygon simplification
   1. Visvalingam algorithm
   2. Douglas Peucker algorithm
 2. Polygon I/O
   1. Text-based points file (.pof): a two column list of points. First columns represent x's and second columns represent y's
   2. Well-Known-Text (.wkt): reads and writes polygons using WKT notation
 3. Polygon comparison
   1. Haussdorf metric between point sets of vertexes
   2. Chamfer metric between points sets of vertexes
   3. Polis metric between polygons
   4. Jaccard Index between polygons
