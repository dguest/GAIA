/*------------------------------------------------------
				Layer.h
				By: Luke de Oliveira

This is the class that holds a synapse structure to 
mediate the inter-layer dot-products. In addition, this
class performs the actual implementation of the stacked, 
denoising auto-encoders. All sub-methods are implemented 
using STL containers, but must be compiled with a 
compiler that supports range-based for loops, 
unique_ptr<T>, and std::move semantics.

------------------------------------------------------*/

#ifndef LAYER_H
#define LAYER_H 

#include <string>
#include <random>
#include <cmath>
#include "Activation.h"
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <memory>

class Layer
{
public:
//----------------------------------------------------------------------------
	Layer(int ins, int outs, bool last, std::vector<double> (*Activation_function)(std::vector<double>)/*, std::mt19937_64 Generator = val_gen::inclusion_generator*/);
	Layer(std::vector<std::vector<double> > Synapse, bool last);
	~Layer();
	std::vector<double> fire();
	void setDelta(std::vector<double> deltas);
	void perturb(double epsilon);
	void resetWeights(double bound);
	void make_denoising();
	unsigned int &include_node(const int node);
	void reset_inclusion();
	void encode(std::vector<double> input, double learning, double weight);
	void feed(std::vector<double> event);
	void set(int i, int j, double val);
	void drop();
	void setMomentum(double x);
	std::vector<double> getReconstructedInput(std::vector<double> jet);
	void weight_dropout(double prob_out);

private:
//----------------------------------------------------------------------------
	friend class Architecture;
	friend class NeuralNet;
	// std::mt19937_64 generator;
	std::vector<std::vector<double> > Synapse, DeltaSynapse;
	// Layer *Auto_Encoder;
	std::vector<unsigned int> Dropout;
	std::unique_ptr<Layer> Auto_Encoder;
	std::vector<double> Delta, Outs;
	std::vector<double> (*_sigmoid)(std::vector<double>);
	
	int ins, outs;
	bool last;
	double gamma, onemingamma;
};

//----------------------------------------------------------------------------
//------------------ NON CLASS UTILITY-TYPE FUNCTIONS ------------------------
//----------------------------------------------------------------------------

void progress_bar(int percent);
void epoch_progress_bar(int percent, int epoch, int tot);

#endif



