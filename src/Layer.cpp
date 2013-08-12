//------------------------------------------------------
//				Layer.h
//				By: Luke de Oliveira
//------------------------------------------------------

#include "Layer.h"

std::mt19937_64 generator;

//----------------------------------------------------------------------------
Layer::Layer(int ins, int outs, bool last, 
	         std::vector<double> (*Activation_function)(std::vector<double>)): 
			 ins(ins), outs(outs), last(last), Outs(outs, 0.00), 
             Delta(outs, 0.00), _sigmoid(Activation_function)

{
	std::vector<double> new_vec(outs, 0.00);
	for (int i = 0; i < ins + 1; ++i) 
	{
		Synapse.push_back(new_vec);
		DeltaSynapse.push_back(new_vec);
	}
	resetWeights(0.1);
	setMomentum(0.9);
}

//----------------------------------------------------------------------------
Layer::Layer(std::vector<std::vector<double> > Synapse, bool last) : Synapse ( Synapse ), Outs(outs, 0.00), Delta(outs, 0.00) 
{
	std::vector<double> new_vec(outs, 0.00);
	for (int i = 0; i < ins + 1; ++i) 
	{
		DeltaSynapse.push_back(new_vec);
	}
}

//----------------------------------------------------------------------------
Layer::~Layer() 
{
	// delete Auto_Encoder;
}

//----------------------------------------------------------------------------
std::vector<double> Layer::fire() 
{
	return Outs;
}

//----------------------------------------------------------------------------
void Layer::perturb(double epsilon) 
{
	std::uniform_real_distribution < double > distribution(-epsilon, epsilon);

	for(auto &i : Synapse) 
	{
		for(auto &ij : i) 
		{
			ij += distribution(generator);
		}
	}
}

//----------------------------------------------------------------------------
void Layer::resetWeights(double bound) 
{
	std::uniform_real_distribution < double > distribution(-bound, bound);
	for(auto &i : Synapse) 
	{
		for(auto &ij : i) 
		{
			ij = distribution(generator);
		}
	}
	for(auto &i : DeltaSynapse) 
	{
		for(auto &ij : i) 
		{
			ij = 0;
		}
	}
	// if (Auto_Encoder)
	// {
	// 	Auto_Encoder->resetWeights(bound);
	// }
}

//----------------------------------------------------------------------------
void Layer::setDelta(std::vector<double> deltas) 
{
	Delta = deltas;
}

//----------------------------------------------------------------------------

void Layer::make_denoising()
{
	Auto_Encoder = std::move(std::unique_ptr<Layer>(new Layer(outs, ins, last, _sigmoid)));
	// Auto_Encoder = std::move(std::unique_ptr<Layer>(new Layer(outs - 1, ins, last, _sigmoid)));
	// Auto_Encoder->setMomentum(.5);
}

//----------------------------------------------------------------------------

void Layer::encode(std::vector<double> input, double learning, double weight)
{
	// std::cout << Auto_Encoder->gamma << ", " << Auto_Encoder->onemingamma << std::endl;
	/* 
	Here, we use a denoising autoencoder to determine initial weights in the 
	neural network. We essentially perform  non-linear PCA at each layer by 
	estimating the identity function through a series of sigmoidal transforms.
	*/
	// std::cout << "\n";
	// vector_print(input);
	std::vector<double> identity_estimate(input);
	// std::vector<double> representation(outs, 0.00);

	input.push_back(1); // Add the bias term to the vector.

	double sum;
	// Feed throight the first layer
	for (int i = 0; i < outs; ++i) {
		sum = 0;
		for (int j = 0; j <= ins; ++j) {
			// Computes the dot product.
			sum += input.at(j) * (Synapse.at(j).at(i)); 
		}
		Outs.at(i) = sum;
	}

	Outs = _sigmoid(Outs); // transform the output of the first synapse.

	// auto Outs_copy(Outs);
	// Outs_copy.push_back(1);

	// Feed throight the second layer
	for (int i = 0; i < ins; ++i) {
		sum = 0;
		for (int j = 0; j < outs; ++j) {
			sum += Outs.at(j) * (Auto_Encoder->Synapse.at(j).at(i));
		}
		identity_estimate.at(i) = sum;
		Auto_Encoder->Outs.at(i) = sum;
	}
	// std::cout << "Error before: " << sum << ", ";
	// std::cout << "Initial Vector" << std::endl;
	// vector_print(input);
	// std::cout << "Initial estimate Vector" << std::endl;
	// vector_print(identity_estimate);

	std::vector<double> error(identity_estimate);  /* needs to be (estimated - actual) */

	for (int i = 0; i < ins; ++i) // Make error into the partial derivative.
	{
		// std::cout << "error.at(i) = " << error.at(i) << std::endl;
		// std::cout << "input.at(i) = " << input.at(i) << std::endl;
		// std::cout << "dsig(identity_estimate.at(i)) = " << dsig(identity_estimate.at(i)) << std::endl;
		error.at(i) -= input.at(i);
		error.at(i) = dsig(error.at(i)) * (error.at(i) - input.at(i));
		// error.at(i) = error.at(i) * (identity_estimate.at(i));
	}

	Auto_Encoder->setDelta( error );
	// vector_print(Auto_Encoder->Delta);

	for (int i = 0; i < outs; ++i) 
	{ // Delta = DSIG * Synapse * prev_Delta
		double val = 0;
		for (int j = 0; j < ins; ++j) 
		{
			val += dsig(Outs.at(i)) * (Auto_Encoder->Synapse.at(i).at(j)) * (Auto_Encoder->Delta.at(j));
		}
		Delta.at(i) = val;
	}
	// std::cout << "\n";
	// vector_print(Delta);
	// std::cout << "\nouts = " << outs << ", " << Auto_Encoder->Synapse.size();
	// std::cout << "ins = " << ins << ", " << Auto_Encoder->Synapse.at(0).size();
	// std::cin.ignore();
	for (int i = 0; i < ins; ++i) 
	{
    	int j;
    	for (j = 0; j <  outs; ++j) //not plus one because of bias 	
    	{
    		Auto_Encoder->set(j, i, -learning * weight * Auto_Encoder->Delta.at(i) * Outs.at(j));
    	}  	
    	// Auto_Encoder->set(j, i, -learning * weight * Auto_Encoder->Delta.at(i));
    }

    for (int i = 0; i < outs; ++i) 
    {
    	int j;
    	//not plus one because of bias 
    	for (j = 0; j < ins; ++j)
    	{
    		set(j, i, -learning * weight * Delta.at(i) * input.at(j));
    	}
    	set(j, i, -learning * weight * Delta.at(i));
    }
    drop();
    Auto_Encoder->drop();
 //    sum = 0;
    // std::cout << "Final Vector" << std::endl;

	// Feed throight the first layer

	for (int i = 0; i < outs; ++i) {
		sum = 0;
		for (int j = 0; j <= ins; ++j) {
			// Computes the dot product.
			sum += input.at(j) * (Synapse.at(j).at(i)); 
		}
		Outs.at(i) = sum;
	}

	Outs = _sigmoid(Outs); // transform the output of the first synapse.

	auto Outs_copy = (Outs);
	Outs_copy.push_back(1);

	// Feed throight the second layer
	for (int i = 0; i < ins; ++i) {
		sum = 0;
		for (int j = 0; j <= outs; ++j) {
			sum += Outs_copy.at(j) * (Auto_Encoder->Synapse.at(j).at(i));
		}
		Auto_Encoder->Outs.at(i) = sum;
	}
    // vector_print(Auto_Encoder->Outs);

	// std::cout << "Error after: " << sum << std::endl;
	// std::cin.ignore();
}


std::vector<double> Layer::getReconstructedInput(std::vector<double> jet)
{
	double sum;
	jet.push_back(1.0);
	for (int i = 0; i < outs; ++i) {
		sum = 0;
		for (int j = 0; j <= ins; ++j) {
			// Computes the dot product.
			sum += jet.at(j) * (Synapse.at(j).at(i)); 
		}
		Outs.at(i) = sum;
	}

	Outs = _sigmoid(Outs); // transform the output of the first synapse.

	auto Outs_copy(Outs);
	Outs_copy.push_back(1);

	// Feed throight the second layer
	for (int i = 0; i < ins; ++i) {
		sum = 0;
		for (int j = 0; j <= outs; ++j) {
			sum += Outs_copy.at(j) * (Auto_Encoder->Synapse.at(j).at(i));
		}
		Auto_Encoder->Outs.at(i) = sum;
	}
	return Auto_Encoder->Outs;
}

//----------------------------------------------------------------------------
void Layer::feed(std::vector<double> event) 
{
	event.push_back(1);
	double sum;
	for (int i = 0; i < outs; ++i) 
	{
		sum = 0;
		for (int j = 0; j <= ins; ++j) 
		{
			sum += event.at(j) * (Synapse.at(j).at(i));
		}
		Outs.at(i) = sum;
	}
	if (!last) 
	{
		Outs = _sigmoid(Outs);
	}
}

//----------------------------------------------------------------------------
void Layer::drop() 
{
	for (int i = 0; i <= ins; ++i) 
	{
		for (int j = 0; j < outs; ++j) 
		{
			Synapse.at(i).at(j) +=  DeltaSynapse.at(i).at(j);
		}
	}
}

//----------------------------------------------------------------------------
void Layer::set(int i, int j, double val) 
{
	DeltaSynapse.at(i).at(j) = (onemingamma * val) + gamma * DeltaSynapse.at(i).at(j);
}

//----------------------------------------------------------------------------
void Layer::setMomentum(double x) 
{
	gamma = x;
	onemingamma = 1.0 - x;
}
