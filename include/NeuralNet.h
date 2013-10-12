//------------------------------------------------------
//				NeuralNet.h
//				By: Luke de Oliveira
//------------------------------------------------------

#ifndef NEURALNET_H
#define NEURALNET_H 

#include <cmath>
#include <random>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include "Architecture.h"
#include "Activation.h"
#include "Dataset.h"
#include <assert.h>


typedef std::pair<std::string, double> DictElement;

class NeuralNet
{
public:
//----------------------------------------------------------------------------
	NeuralNet( std::vector<int> structure);
	NeuralNet();
	~NeuralNet();
	NeuralNet( NeuralNet &A );
	void set_dataset(std::string root_file = "", std::string tree_name = "");
	void get_dataset_entry(const int index);
	bool set_input_branch(std::string name, std::string type);
	bool set_output_branch(std::string name, std::string type);
	bool set_control_branch(std::string name, std::string type);

	double get_value(std::string name);

	bool load_specifications(const std::string &filename);

	double get_physics_reweighting();

	std::vector<double> input();
	std::vector<double> output();

	void setLearning( double x );
	void setMomentum( double x );
	void anneal( double x );

	void encode(std::vector<std::vector<double>> input, std::vector<double> weight, bool verbose);
	void encode(bool verbose = 1);

	void train(int n_epochs, int n_train, std::string save_filename, 
		       bool verbose = 0, std::string timestamp = "", bool memory = false);

	void train(std::vector<double> Event, std::vector<double> Actual, double weight = 1);

	void getTransform(bool verbose = false, bool into_memory = 0, int n_train = -1, bool cdf_weight = false, bool relative = true);
	void setTransform( std::vector<double> Mean, std::vector<double> Stddev );


	void setActivationFunctions(std::vector<double> (*sigmoid_function) (std::vector<double>),
                                double (*sigmoid_derivative)(double), 
                                std::vector<double> (*softmax_function) (std::vector<double>));

	std::vector<double> transform( std::vector<double> Event );
	std::vector<std::vector<double>> transform(std::vector<std::vector<double>> Event);

	std::vector<double> predict( std::vector<double> Event );
	NeuralNet& operator=( const NeuralNet &A );

	bool save( const std::string &filename );
	bool load( const std::string &filename );
	bool write_perf( const std::string &filename = "" , int start = 0, int end = 10000);
	std::vector<std::string> get_ranking();
private:
//----------------------------------------------------------------------------
	std::unique_ptr<Dataset> dataset;
	std::vector<std::vector<double> > dataset_mem, labels_mem;
	std::unique_ptr<Architecture> Net;
	double learning, momentum;
	std::vector<int> structure;
	int count;
	std::vector<double> mean, stddev, weights_mem;
	double (*_sigmoid_derivative) (double);
	std::vector<double> (*_softmax_function) (std::vector<double>);
	std::vector<double> (*_sigmoid) (std::vector<double>);
};



//----------------------------------------------------------------------------
//------------------ NON CLASS UTILITY-TYPE FUNCTIONS ------------------------
//----------------------------------------------------------------------------

template <typename T>
inline T get_row_sum(std::vector<std::vector<T>> Array, unsigned int row_number)
{
	int n_cols = Array[0].size();
	T sum = Array[0][row_number];
	for (int i = 1; i < n_cols; ++i)
	{
		auto temp = (Array.at(row_number).at(i));
		sum += (temp * temp);
	}
	return sum;
}

struct _ranking_comparison {
	bool operator() (DictElement A, DictElement B) 
	{
		return (A.second < B.second);
	}
};





#endif