//------------------------------------------------------
//				Architecture.h
//				By: Luke de Oliveira
//------------------------------------------------------

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H 

#include "Layer.h"
#include <cmath>
#include <string>
#include <random>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <utility>
#include <memory>

class Architecture
{
public:
//----------------------------------------------------------------------------
	Architecture(std::vector<int> structure, std::vector<double> (*sigmoid_function) (std::vector<double>), double (*sigmoid_derivative) (double));
	~Architecture();
	std::vector<double> test(std::vector<double> Event);
	void backpropagate(std::vector<double> error, std::vector<double> Event, double weight);
	void setLearning(double x);
	void make_denoising();
	void weight_dropout(double prob_out = 0.5, double prob_input_layer = 1.0);
	void encode(std::vector<std::vector<double>> input, double learning, std::vector<double> weight, bool verbose, int epochs = 6);
	void setMomentum(double x);
	void anneal(double x);
	std::vector<std::vector<double>> get_first_layer();
private:
//----------------------------------------------------------------------------
	friend class NeuralNet;
	std::vector< std::unique_ptr<Layer> > Bundle;
	std::vector<double> reconstruction_error;
	std::vector<int> structure;
	int layers;
	bool is_denoising;
	double eta, lambda;
	double (*_sigmoid_derivative) (double);
	std::vector<double> (*_sigmoid_function) (std::vector<double>);
	
};
#endif


