//------------------------------------------------------
//				Architecture.h
//				By: Luke de Oliveira
//------------------------------------------------------

#include "Architecture.h"


//----------------------------------------------------------------------------
Architecture::Architecture(std::vector<int> structure, 
	                       std::vector<double> (*sigmoid_function) (std::vector<double>), 
	                       double (*sigmoid_derivative) (double)) : 
                           structure( structure ),  
                           _sigmoid_derivative(sigmoid_derivative), 
                           _sigmoid_function(sigmoid_function), 
                           is_denoising( false )
{
	layers = structure.size();
	int l;
	bool final;
	for (l = 0; l < (layers - 1); ++l) 
	{
		final = ((l == (layers - 2)) ? true : false);
		Bundle.push_back( std::move(std::unique_ptr<Layer>(new Layer(structure.at(l), structure.at(l + 1), final, _sigmoid_function))) );
	}
	layers = Bundle.size();
	lambda = 0;
}
//----------------------------------------------------------------------------
Architecture::~Architecture() 
{
	Bundle.clear();
}

//----------------------------------------------------------------------------
std::vector<double> Architecture::test(std::vector<double> Event) 
{
	Bundle.at(0)->feed((Event));
	unsigned int l;

	for (l = 1; l < (Bundle.size()); ++l)
	{
		Bundle.at(l)->feed((Bundle.at(l - 1)->Outs));
	}
	return std::move(Bundle.at(l - 1)->fire());
}

//----------------------------------------------------------------------------
void Architecture::setLearning(double x) 
{
	eta = x;
}

//----------------------------------------------------------------------------
void Architecture::setMomentum(double x) 
{	
	for (auto &layer : Bundle) 
	{
		layer->setMomentum(x);
	}
}

//----------------------------------------------------------------------------
void Architecture::anneal(double x) 
{
	for (auto &layer : Bundle) 
	{
		layer->perturb(x);
	}
}

//----------------------------------------------------------------------------
void Architecture::backpropagate(
	std::vector<double> error,  /* needs to be (estimated - actual) */
	std::vector<double> Event,
	double weight ) 
{
	Bundle.at(layers - 1)->setDelta( error ); 
	double val;

	for (int l = layers - 1; l > 0; l--) 
	{ //for each layer in the neural net
		for (int i = 0; i < structure.at(l); ++i) 
		{ // Delta = DSIG * Synapse * prev_Delta
			val = 0;
			for (int j = 0; j < structure.at(l + 1); ++j) 
			{
				val += _sigmoid_derivative(Bundle.at(l - 1)->Outs.at(i)) * (Bundle.at(l)->Synapse.at(i).at(j)) * (Bundle.at(l)->Delta.at(j));
			}
			Bundle.at(l - 1)->Delta.at(i) = val;
		}
	}

	for (int l = layers - 1; l > 0; l--) 
	{ //for each layer in the neural net
		for (int i = 0; i < structure.at(l + 1); ++i) 
		{
	    	int j;
	    	for (j = 0; j <  structure.at(l); j++) //not plus one because of bias 	
	    	{
	    		Bundle.at(l)->set(j, i, weight * (-eta * Bundle.at(l)->Delta.at(i) * Bundle.at(l - 1)->Outs.at(j) - lambda * Bundle.at(l)->Synapse.at(j).at(i)));
	    	}
	    	Bundle.at(l)->set(j, i, -eta * weight * Bundle.at(l)->Delta.at(i));
	    }
	}
    for (int i = 0; i < Bundle.at(0)->outs; ++i) 
    {
    	int j;
    	//not plus one because of bias 
    	for (j = 0; j < Bundle.at(0)->ins; j++)
    	{
    		Bundle.at(0)->set(j, i, weight * (-eta * Bundle.at(0)->Delta.at(i) * Event.at(j) - lambda * Bundle.at(0)->Synapse.at(j).at(i)));
    	}
    	Bundle.at(0)->set(j, i, -eta * weight * (Bundle.at(0)->Delta.at(i)));
    }
    for (auto &layer : Bundle) 
    {
		layer->drop();
	}
}
//----------------------------------------------------------------------------
void Architecture::make_denoising()
{
	for (auto &layer : Bundle)
	{
		layer->make_denoising();
	}
	is_denoising = true;
}
//----------------------------------------------------------------------------
void Architecture::encode(std::vector<std::vector<double>> input, double learning, std::vector<double> weight, bool verbose, int epochs)
{

	if (verbose)
	{
		std::cout << "\nTraining stacked Denoising auto-encoders:\n";
	}
	if (!is_denoising)
	{
		make_denoising();
	}
	double pct;
	unsigned int idx = 0, ctr = 0, total_passes = (input.size() * layers * epochs);
	for(int epoch = 0; epoch < epochs; ++epoch)
	{
		idx = 0;
		for (auto jet : input) // The first layer needs to be done by itself.
		{
			std::cout << "Original: " << std::endl;
			vector_print(jet);
			Bundle.at(0)->encode(jet, learning, weight.at(idx)); // Initialize the first layer of PCA
			std::cout << "Reconstructed" << std::endl;
			vector_print(Bundle.at(0)->getReconstructedInput(jet));
			++idx;
			++ctr;
			
		}
		if (verbose)
        {
            pct = (((double)(ctr)) / ((double) (total_passes))) * 100;
            progress_bar(pct);

        }
	}
	for (int l = 1; l < layers; ++l) // now, for each of the following layers...
	{
		for(int epoch = 0; epoch < epochs; ++epoch)
		{
			idx = 0;
			for (auto jet : input) // ...we consider each jet
			{
				Bundle.at(0)->feed(jet);
				for (int k = 1; k < l; ++k) // feed the tuned input up....
				{
					Bundle.at(k)->feed((Bundle.at(k - 1)->Outs));
				}
				Bundle.at(l)->encode(Bundle.at(l - 1)->Outs, learning, weight.at(idx)); // and encode the layer in question.
				++idx;
				++ctr;
				if (verbose)
		        {
		            pct = (((double)(ctr)) / ((double) (total_passes))) * 100;
		            progress_bar(pct);
		        }
		        // std::cout << "Original: " << std::endl;
				// vector_print(jet);
				// std::cout << "Reconstructed" << std::endl;
				// vector_print(Bundle.at(0)->getReconstructedInput(jet));
			}
		}
	}
}

std::vector<std::vector<double>> Architecture::get_first_layer()
{
	return Bundle.at(0)->Synapse;
}





