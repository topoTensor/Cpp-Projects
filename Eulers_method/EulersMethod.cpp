#include <vector>
#include <functional>
#include <Eigen/dense>

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

using namespace Eigen;
using json = nlohmann::json;

template<unsigned int N, typename F>
std::vector<std::pair<double, Vector<double, N>>> Euler_history(double t_0, double T, double h, const Ref<const Vector<double, N>>& ys, F f) {
	// F is of the form f(t,y,y',y'', ..., y^(N))

	std::vector<std::pair<double, Vector<double, N>>> history;
	history.emplace_back(t_0, ys);
	
	// ys is a vector of derivatives y, y', y'', ..., y^(N)
	// y[N - 1] represents the N-th derivative (highest order term)
	Vector<double, N> y = ys;

	double t = t_0;

	while (t+h <= T) {
		const auto y_orig = y;
		y.head(N - 1) += h * y.tail(N-1);
		y[N - 1] += h * f(t, y_orig);

		t += h;
		history.emplace_back(t, y);
	}
	return history;
}

template<unsigned int N, typename F>
Vector<double, N> Euler(double t_0, double T, double h, const Ref<const Vector<double, N>>& ys, F f) {
	// F is of the form f(t,y,y',y'', ..., y^(N))

	// ys is a vector of derivatives y, y', y'', ..., y^(N)
	// y[N - 1] represents the N-th derivative (highest order term)
	Vector<double, N> y = ys;

	double t = t_0;

	while (t + h <= T) {
		const auto y_orig = y;
		y.head(N - 1) += h * y.tail(N - 1);
		y[N - 1] += h * f(t, y_orig);

		t += h;
	}
	return y;
}


double Euler_first_order(double t_0, double T, double h, double y_0, std::function<double(double, double)> f) {
	// function f is of the form f(t,x)
	// y'(t) = f(t,y(t))

	/* 
		using y'(t) = ( y(t+h) - y(t) ) / h we get
		y(t+h) = y(t) + h * y'(t)

		then by substituting for y'(t)
		y(t+h) = y(t) + h * f(t, y(t))

		recursively we get
		
		y_(n+1) = y_n + h * f(t_n, y_n)

	 */

	double t_n = t_0;
	double y_n = y_0;

	while (t_n < T) {
		y_n += h * f(t_n, y_n);
		t_n += h;
	}

	return y_n;
}

template<unsigned int N>
void print_history(std::vector<std::pair<double, Vector<double, N>>> history, bool full, int last_n) {
	if (full) {
		last_n = history.size();
	}
	for (int i = history.size() - last_n; i < history.size(); i++) {
		std::cout << history[i].first << " ( " << history[i].second.transpose() << " ) " << "\n";
	}
}
template<unsigned int N>
void write_history(std::vector<std::pair<double, Vector<double, N>>> history, json& document) {
	for (const std::pair<double, Vector<double, N>>& q : history) {
		document.push_back({
			{"t", q.first},
			{"y", q.second}
			});
	}
}

int main() {
	//const unsigned int N = 3;

	//auto f = [](double t, auto&& ys)->double {
	//	return sin(t) + cos(t) * ys[0] + t * t * ys[1] - 4 * t * ys[2];
	//	};
	//Vector<double, N> ys;
	//ys << 2, -1, 3;
	///*auto history = Euler_history<N>(0.0, 5, 0.01, ys, f);
	//

	//const int k = 5;
	//for (int i = history.size() - k; i < history.size(); i++) {
	//	std::cout << history[i].first << " (" << history[i].second.transpose() << ")" << "\n";
	//}*/
	//double y = Euler<3>(0.0, 5, 0.01, ys, f)[0];
	//std::cout << y << std::endl;

	/*
		d^2x/dt^2=-kx/m
		=>
		y''=-ky/m, where f(t,y)=-ky/m
	*/

	const double k = 0.1;
	const double m = 1.0;

	auto f = [](double t, auto&& ys) {
		return -k * ys[0] / m;
		};

	Vector2d ys;
	ys << 0.5, 0.5;

	auto history = Euler_history<2>(0.0, 100.0, 0.1, ys, f);
	//print_history(history, false, 5);

	json history_json;
	write_history(history, history_json);

	std::ofstream file("history.json");
	file << history_json.dump();
	file.close();

	//std::cout << history_json << std::endl;
}