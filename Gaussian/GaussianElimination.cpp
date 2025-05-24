#include <iostream>
#include <Eigen/dense>
#include <memory>

using namespace std;
using namespace Eigen;

void Gaussian_Elimination(Ref<MatrixXf> mat) {
	// marix is supposed to be augmented with vector
	const unsigned int n = mat.rows();
	for (int i = 0; i < n-1; i++) {
		if (mat(i, i) == 0) {
			throw "pivot is zero";
		}
		for (int j = i + 1; j < n; j++) {
			mat.row(i) *= -mat(j, i) / mat(i, i);
			mat.row(j) += mat.row(i);
			mat = (mat.array().abs() < 1e-5).select(0, mat);
			//cout << mat << endl << endl;
		}
		//cout << i << endl;
	}
}

VectorXf Back_Substitution(Ref<MatrixXf> mat) {
	// marix is supposed to be augmented with vector

	const unsigned int n = mat.rows();
	VectorXf x = VectorXf::Zero(n);

	for (int i = 0; i < n; i++) {
		float q = 0;
			//cout << "x\n" << x << endl;
			//cout << "mat\n" << mat(n - i - 1, seqN(0, n)).reverse() << endl;

		if (i != 0) {
			q = mat(n - i - 1, seqN(0, n)).reverse() *x;
		}
		//cout << q << endl;

		x(i) = (mat(n-i-1, n) - q) / mat(n - i-1, n - i-1);
	}
	return x.reverse();
}

int Gaussian_main() {
	Matrix4f mat;
	mat << 3, 5, 7, 9,
		2, 8, 4, 6,
		1, 3, 9, 2,
		4, 6, 8, 5;

	Eigen::Vector4f v;
	v << 10, 20, 30, 40;

	Matrix<float, mat.rows(), mat.rows()+1> augment;

	augment << mat,v;
	cout << augment << endl;
	
	try {
		Gaussian_Elimination(augment);
	} catch (const char * e) {
		cout << e << endl;
		abort();
	}
	cout << augment << endl;

	cout << Back_Substitution(augment);
}