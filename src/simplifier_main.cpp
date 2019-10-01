#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "polygon.hpp"
#include "simplifier.hpp"

#include "cxxopts.hpp"

Mat genImage(bool drawWindow) {
	Mat src = Mat::ones(globals.max_y * 4 * FACTOR, globals.max_x * 4 * FACTOR, CV_8U)*255;
	cvtColor(src, src, COLOR_GRAY2BGR);
	namedWindow("Polygons", WINDOW_NORMAL);

	//Originals
	drawPolygon(src, globals.p1, Scalar(0, 0, 0), 0, 0, false);
	drawPolygon(src, globals.p1, Scalar(0, 0, 0), 0, 2, true);

	drawPolygon(src, globals.p2, Scalar(0, 0, 0), 2, 0, false);
	drawPolygon(src, globals.p2, Scalar(0, 0, 0), 2, 2, true);

	std::fstream fs("p1_orig.pof", std::fstream::out);
	globals.p1.save(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("p2_orig.pof", std::fstream::out);
	globals.p2.save(fs, Polygon::FileType::FILE_POF);

	//Visvalingam
	Polygon vv_p1 = globals.p1, vv_p2 = globals.p2;
	Simplifier::visvalingam_until_n(vv_p1, globals.red_per);
	Simplifier::visvalingam_until_n(vv_p2, globals.red_per);

	fs = std::fstream("p1_vv.pof", std::fstream::out);
	vv_p1.save(fs, Polygon::FileType::FILE_POF);
	fs.close();
	fs = std::fstream("p2_vv.pof", std::fstream::out);
	vv_p2.save(fs, Polygon::FileType::FILE_POF);

	drawPolygon(src, vv_p1, Scalar(0, 0, 0), 0, 1, false);
	drawPolygon(src, vv_p1, Scalar(0, 0, 0), 0, 3, true);

	drawPolygon(src, vv_p2, Scalar(0, 0, 0), 2, 1, false);
	drawPolygon(src, vv_p2, Scalar(0, 0, 0), 2, 3, true);

	//Douglas-Peucker
	Polygon dp_p1 = globals.p1, dp_p2 = globals.p2;
	Simplifier::douglas_peucker_until_n(dp_p1, globals.red_per);
	Simplifier::douglas_peucker_until_n(dp_p2, globals.red_per);

	fs = std::fstream("p1_dp.pof", std::fstream::out);
	dp_p1.save(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("p2_dp.pof", std::fstream::out);
	dp_p2.save(fs, Polygon::FileType::FILE_POF);

	drawPolygon(src, dp_p1, Scalar(0, 0, 0), 1, 1, false);
	drawPolygon(src, dp_p1, Scalar(0, 0, 0), 1, 3, true);

	drawPolygon(src, dp_p2, Scalar(0, 0, 0), 3, 1, false);
	drawPolygon(src, dp_p2, Scalar(0, 0, 0), 3, 3, true);

	//Temporal Visvalingam
	std::vector<Polygon> vvt_pols;
	vvt_pols.push_back(globals.p1);
	vvt_pols.push_back(globals.p2);
	Simplifier::visvalingam_with_time(vvt_pols, globals.red_per, globals.t_value);
	fs = std::fstream("p1_vvt.pof", std::fstream::out);
	vvt_pols[0].save(fs, Polygon::FileType::FILE_POF);
	fs = std::fstream("p2_vvt.pof", std::fstream::out);
	vvt_pols[1].save(fs, Polygon::FileType::FILE_POF);

	drawPolygon(src, vvt_pols[0], Scalar(0, 0, 0), 1, 0, false);
	drawPolygon(src, vvt_pols[0], Scalar(0, 0, 0), 1, 2, true);
	
	drawPolygon(src, vvt_pols[1], Scalar(0, 0, 0), 3, 0, false);
	drawPolygon(src, vvt_pols[1], Scalar(0, 0, 0), 3, 2, true);

	if (drawWindow)
		imshow("Polygons", src);
	return src;
}

int main(int argc, char *argv[]) {
	cxxopts::Options options("Geometry Simplifier", "Simplifies two given polygons, with options to export geometries.");
	options.add_options()
		("h,help", "Shows full help")
		("p", "First polygon to be simplified (WKT).", cxxopts::value<std::string>())
		("q", "Second polygon to be simplified (WKT).", cxxopts::value<std::string>())
		("po", "First polygon output file (WKT).", cxxopts::value<std::string>())
		("qo", "Second polygon output file (WKT).", cxxopts::value<std::string>())
		("r", "Reduction percentage of points.", cxxopts::value<double>())
		("m", "Method. 1=Rammer-Douglas-Peucker, 2=Visvalingan-Whyatt, 3=Time-complatible", cxxopts::value<int>())
		("t", "Time-value for VVT method.", cxxopts::value<double>());
	
	if (argc==1) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	auto result = options.parse(argc, argv);

	if (result["help"].as<bool>() || !result.count("m")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	switch (result["m"].as<int>()) {
		case 1: //RDP
			std::fstream fs(result["p"].as<std::string>(), std::fstream::in);
			std::fstream fs2(result["q"].as<std::string>(), std::fstream::in);
			break;

		case 2: //VW

			break;

		case 3: //VWT
			break;

	default:
			std::cout << option.help() << std::endl;
			return 0;
	}

	std::fstream fs(result["p"].as<std::string>(), std::fstream::in);
	std::fstream fs2(result["q"].as<std::string>(), std::fstream::in);

	if (!fs.is_open()) {
		std::cout << "Error, could not load one of the files\n";
		exit(2);
	}

	Polygon p1;
	if (fs.peek() == 'P') 
		p1 = Polygon(fs, Polygon::FileType::FILE_WKT);
	else
		p1 = Polygon(fs);

	Polygon p2;
	if (fs2.peek() == 'P')
		p2 = Polygon(fs2, Polygon::FileType::FILE_WKT);
	else
		p2 = Polygon(fs2);

	globals.p1 = p1;
	globals.p2 = p2;

	for (SimplePoint p: p1.points) {
		if (p.x > globals.max_x) globals.max_x = p.x;
		if (p.y > globals.max_y) globals.max_y = p.y;
	}
	for (SimplePoint p: p2.points) {
		if (p.x > globals.max_x) globals.max_x = p.x;
		if (p.y > globals.max_y) globals.max_y = p.y;
	}
	if (!result.count("output")) { //Show window
		genImage(true);

		createTrackbar("\% points to remove:", "Polygons", nullptr, 100, change_red_per);
		createTrackbar("time weigth:", "Polygons", nullptr, 100, change_t_value);

		char c;
		while ((c = waitKey()) != 'q') {
			continue;
		}
	} else {
		globals.red_per = result["r"].as<double>();
		globals.t_value = result["t"].as<double>();

		Mat img = genImage(false);
		imwrite(result["o"].as<std::string>(), img);
	}
}
