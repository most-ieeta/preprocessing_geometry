#include <iostream>
#include <fstream>
#include <istream>
#include "polygon.hpp"
//#include "aligner.hpp"
#include "icpPointToPoint.h"

int main(int, char* argv[]) {
	std::ifstream if1(argv[1]);
	Polygon p1;
	if (if1.peek() == 'P') {
		p1 = Polygon(if1, Polygon::FileType::FILE_WKT);
	} else {
		p1 = Polygon(if1, Polygon::FileType::FILE_POF);
	}

	std::ifstream if2(argv[2]);
	Polygon p2;
	if (if2.peek() == 'P') {
		p2 = Polygon(if2, Polygon::FileType::FILE_WKT);
	} else {
		p2 = Polygon(if2, Polygon::FileType::FILE_POF);
	}

	double* M = (double*)calloc(2*p1.points.size(), sizeof(double));
	double* T = (double*)calloc(2*p2.points.size(), sizeof(double));

	size_t k;
	k = 0;
	for (auto p: p1.points) {
		M[k*2+0] = p.x;
		M[k*2+1] = p.y;
		k++;
	}

	k = 0;
	for (auto p: p2.points) {
		T[k*2+0] = p.x;
		T[k*2+1] = p.y;
		k++;
	}

	Matrix R = Matrix::eye(2); //2 dims
	Matrix t(2, 1);

	IcpPointToPoint icp(M, p1.points.size(), 2);
	icp.fit(T, p2.points.size(), R, t, -1);

	k = 0;
	for (size_t i = 0; i < p2.points.size(); ++i) {
		double x=T[k*2+0], y=T[k*2+1];
		std::cout << (x * R.val[0][0] + y * R.val[0][1] + t.val[0][0]) << " "
         << (x * R.val[1][0] + y * R.val[1][1] + t.val[1][0]) << std::endl;
    k++;
	}

	free(M);
	free(T);

	return 0;
}
