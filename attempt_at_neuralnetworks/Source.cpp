
#include <iostream>
#include <initializer_list>
#include <Eigen/dense>
#include <random>

using namespace std;
using namespace Eigen;

enum class ActivationFunctions
{
	sigmoid
};

enum class CostFunctions
{
	mse
};

template<unsigned int N>
class NeuralNet {
public:

	array<int, N> sizes;

	array<MatrixXd, N-1> weights;
	array<VectorXd, N-1> biases;
	array<VectorXd, N> layers;
	array<VectorXd, N> layers_activated;

	bool debug_feedforward;

	ActivationFunctions activation_func;
	CostFunctions cost_func;

	double lr;

	NeuralNet(array<int, N> sizes, bool debug_feedforward, double learning_rate, ActivationFunctions activation_func, CostFunctions cost_func) : debug_feedforward(debug_feedforward), activation_func(activation_func), cost_func(cost_func), lr(learning_rate){
		this->sizes = sizes;

		// LAYERS
		for (int i = 0; i < N; i++) {
			layers[i]=VectorXd(sizes[i]);
			layers_activated[i]=VectorXd(sizes[i]);
		}

		cout << "1. Layers initialized" << endl;

		// BIASES
		for (int i = 0; i < N-1; i++) {
			biases[i] = VectorXd::Zero(sizes[i+1]);
		}

		cout << "2. Biases initialized" << endl;

		// WEIGHTS
		for (int i = 0; i < N-1; i++) {

			random_device rd;
			mt19937 gen(rd());
			normal_distribution<double> dis(0, 1 / (double)sizes[i + 1]);

			MatrixXd random_init_weights = MatrixXd::Zero(sizes[i+1], sizes[i]).unaryExpr([&](double dummy) {return dis(gen); });

			weights[i]=random_init_weights;

			//cout << random_init_weights << endl;
			//cout << this->weights[i].rows() << " : " << this->weights[i].cols() << endl;
		}
		cout << "3. Weights initialized" << endl;

		cout << "Weights: " << weights.size() << ", biases: " << biases.size() << ", layers: " << layers.size() << endl;
		cout << endl;
	}



	void feed_forward(const Ref<const VectorXd>& input) {
		if (debug_feedforward) {
			cout << "Feed forward process: " << endl;
			cout << "input: " << input.transpose() << endl;
		}

		layers[0] = input;
		layers_activated[0] = input;
		for (int i = 0; i < N-1; i++) {
			layers[i + 1] = (weights[i]) * (layers[i]) + biases[i];
			layers_activated[i + 1] = activate(layers[i + 1]);

			if (debug_feedforward) {
				cout << "layer " << i + 1 << " = " << layers_activated[i + 1].transpose() << endl;
			}
		}
	}

	pair<array<MatrixXd, N-1>, array<VectorXd, N >> compute_derivatives(const Ref< const VectorXd> &input, const Ref< const VectorXd>& y) {
		feed_forward(input);

		array<VectorXd, N> deltas;
		array<MatrixXd, N-1> weight_costs;

		deltas[N-1] = cost_deriv(layers_activated[N-1], y).array() * activation_deriv(layers[N-1]).array();

		for (int l = 1; l < N - 1; l++) {
			deltas[N - l - 1] = (weights[N - l - 1].transpose() * deltas[N - l]).array() * activation_deriv(layers[N - l - 1]).array();
		}

		for (int l = 0; l < N - 1; l++) {

			weight_costs[l] = layers_activated[l]*deltas[l + 1].transpose();
		}

		return make_pair(weight_costs, deltas);
	}

	void back_propagation(const Ref< const VectorXd>& input, const Ref< const VectorXd>& y_train) {
		
		array<MatrixXd, N-1> weight_costs;
		array<VectorXd, N> bias_costs;

		auto costs = compute_derivatives(input, y_train);
		weight_costs = costs.first;
		bias_costs = costs.second;

		for (int l = 0; l < N - 1; l++) {
			weights[l] -= lr * weight_costs[l];
			biases[l] -= lr * bias_costs[l];
		}
	}

private:
	VectorXd activate(const Ref<const VectorXd>& input) {
		switch (activation_func) {
		case ActivationFunctions::sigmoid:
			return (1.0 / (1.0 + (-input.array()).exp())).matrix();

		default:
			throw std::invalid_argument("Unknown activation_func function");
		}
	}

	VectorXd activation_deriv(const Ref<const VectorXd>& input) {
		switch (activation_func) {
		case ActivationFunctions::sigmoid:
			return activate(input).array() * (1 - activate(input).array());
		default:
			throw std::invalid_argument("Unknown activation_func function");
		}
	}

	VectorXd cost(const Ref<const VectorXd>& input, const Ref<const VectorXd>& y) {
		switch (cost_func) {
		case CostFunctions::mse:
			return Eigen::pow((input - y).array(), 2).mean();
		default:
			throw std::invalid_argument("Unknown cost function");
		}
	}
	VectorXd cost_deriv(const Ref<const VectorXd>& input, const Ref<const VectorXd>& y) {
		switch (cost_func) {
		case CostFunctions::mse:
			return input - y;
		default:
			throw std::invalid_argument("Unknown cost function");
		}
	}

};

int main() {
	srand(time(NULL));
	NeuralNet<3> nn({ 1,2,3 }, true, 0.01, ActivationFunctions::sigmoid, CostFunctions::mse);
	Vector<double, 1> inp{ 1 };
	Vector<double, 3> y_train{ 1,2,3 };
	nn.back_propagation(inp, y_train);
}