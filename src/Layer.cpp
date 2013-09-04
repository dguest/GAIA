//------------------------------------------------------
//				Layer.h
//				By: Luke de Oliveira
//------------------------------------------------------

#include "Layer.h"

std::mt19937_64 generator;

//----------------------------------------------------------------------------
Layer::Layer(int ins, int outs, bool last, 
	         std::vector<double> (*Activation_function)(std::vector<double>)/*, 
	         std::mt19937_64 Generator*/): 
			 ins(ins), outs(outs), last(last), Outs(outs, 0.00), 
             Delta(outs, 0.00), _sigmoid(Activation_function), 
             Dropout(ins, 1)/*, generator( Generator )*/, DropConnect( dropconnect )

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
Layer::Layer(std::vector<std::vector<double> > Synapse, bool last) : 
             Synapse ( Synapse ), 
             Outs(outs, 0.00), 
             Delta(outs, 0.00) 
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
}
//----------------------------------------------------------------------------
bool Layer::include_node(int i, int j)
{
	return Dropout.at(i).at(j);
}

//----------------------------------------------------------------------------
void Layer::reset_inclusion()
{
	for (auto &i : Dropout)
	{
		for (auto &ij : i)
		{
			ij = true;
		}
	}
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
void Layer::weight_dropout(double prob_out)
{	
	std::bernoulli_distribution determine_dropout(prob_out);
	for (auto &i : Dropout)
	{
		for (auto &ij : i)
		{
			determine_dropout(generator);
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
	Auto_Encoder = std::move(std::unique_ptr<Layer>(new Layer(outs, 
		                                                      ins, 
		                                                      last, 
		                                                      _sigmoid)));
}

//----------------------------------------------------------------------------

void Layer::encode(std::vector<double> input, double learning, double weight)
{
	/* 
	Here, we use a denoising autoencoder to determine initial weights in the 
	neural network. We essentially perform  non-linear PCA at each layer by 
	estimating the identity function through a series of sigmoidal transforms.
	*/
	std::vector<double> identity_estimate(input);

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

	// Feed throight the second layer
	for (int i = 0; i < ins; ++i) {
		sum = 0;
		for (int j = 0; j < outs; ++j) {
			sum += Outs.at(j) * (Auto_Encoder->Synapse.at(j).at(i));
		}
		identity_estimate.at(i) = sum;
		Auto_Encoder->Outs.at(i) = sum;
	}

	std::vector<double> error(identity_estimate);  /* needs to be (estimated - actual) */

	for (int i = 0; i < ins; ++i) // Make error into the partial derivative.
	{
		error.at(i) -= input.at(i);
	}

	Auto_Encoder->setDelta( error );

	for (int i = 0; i < outs; ++i) 
	{ // Delta = DSIG * Synapse * prev_Delta
		double val = 0;
		for (int j = 0; j < ins; ++j) 
		{
			val += dsig(Outs.at(i)) * (Auto_Encoder->Synapse.at(i).at(j)) * (Auto_Encoder->Delta.at(j));
		}
		Delta.at(i) = val;
	}
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

}
//----------------------------------------------------------------------------
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
void Layer::feed(std::vector<double> event, bool dropout, double prob) 
{
	event.push_back(1);
	double sum;
	double oneminprob = 1 - prob
	if (!dropout)
	{
		for (int i = 0; i < outs; ++i) 
		{
			sum = 0;
			int j;
			for (j = 0; j <= ins; ++j) 
			{
				sum += event.at(j) * (Synapse.at(j).at(i));
			}
			Outs.at(i) = sum;
		}
	}
	else if ((prob >= 0) && dropout)
	{
		for (int i = 0; i < outs; ++i) 
		{
			sum = 0;
			int j;
			for (j = 0; j <= ins; ++j) 
			{
				sum += (event.at(j) * (Synapse.at(j).at(i))) * prob;
			}
			Outs.at(i) = sum;
		}
		auto Var ( Outs );
		for (int i = 0; i < outs; ++i)
		{
			Var.at(i) *= (Var.at(i) * oneminprob);
			Var.at(i) /= prob;
			std::normal_distribution<double> distribution(Outs.at(i),Var.at(i));
			Outs.at(i) = distribution( generator );
		}

		for (int i = 0; i < outs; ++i)
		{
			Var.at(i) *= (Var.at(i) * oneminprob);
			Var.at(i) /= prob;
		}
	}
	else
	{
		for (int i = 0; i < outs; ++i) 
		{
			sum = 0;
			int j;
			for (j = 0; j <= ins; ++j) 
			{
				if (include_node(j, i))
				{
					sum += event.at(j) * (Synapse.at(j).at(i));
				}
			}
			Outs.at(i) = sum;
		}
	}
	
	if (!last) 
	{
		Outs = _sigmoid(Outs);
	}
}

//----------------------------------------------------------------------------
void Layer::drop() 
{
	for (int j = 0; j < outs; ++j)
	{
		int i;
		for (i = 0; i <= ins; ++i)
		{
			if (include_node(i, j))
			{
				Synapse.at(i).at(j) +=  DeltaSynapse.at(i).at(j);
			}
		}
		// Synapse.at(i).at(j) +=  DeltaSynapse.at(i).at(j);
	}
}

//----------------------------------------------------------------------------
void Layer::set(int i, int j, double val) 
{
	if (include_node(i, j))
	{
		DeltaSynapse.at(i).at(j) = (onemingamma * val) + gamma * DeltaSynapse.at(i).at(j);	
	}
}

//----------------------------------------------------------------------------
void Layer::setMomentum(double x) 
{
	gamma = x;
	onemingamma = 1.0 - x;
}



//----------------------------------------------------------------------------
//------------------ NON CLASS UTILITY-TYPE FUNCTIONS ------------------------
//----------------------------------------------------------------------------

void progress_bar(int percent) 
{
    std::string _prog_bar;
    for(int i = 0; i < 50; i++) 
    {
        if (i < (percent/2)) 
        {
            _prog_bar.replace(i,1,"=");
        }
        else if (i == (percent/2)) 
        {
            _prog_bar.replace(i,1,">");
        }
        else 
        {
            _prog_bar.replace(i,1," ");
        }
    }
    std::cout<< "\r" "[" << _prog_bar << "] ";
    std::cout.width( 3 );
    std::cout<< percent << "%     " << std::flush;
}
//----------------------------------------------------------------------------
void epoch_progress_bar(int percent, int epoch, int tot) 
{
    std::string _prog_bar;
    for(int i = 0; i < 50; i++) 
    {
        if (i < (percent/2)) 
        {
            _prog_bar.replace(i,1,"=");
        }
        else if (i == (percent/2)) 
        {
            _prog_bar.replace(i,1,">");
        }
        else 
        {
            _prog_bar.replace(i,1," ");
        }
    }
    std::cout << "\r" "Epoch " << epoch << " of " << tot << ", [" << _prog_bar << "] ";
    std::cout.width( 6 );
    std::cout<< std::setprecision(3) << percent << "%     " << std::flush;
}
