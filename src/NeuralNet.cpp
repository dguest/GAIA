//------------------------------------------------------
//				NeuralNet.cpp
//				By: Luke de Oliveira
//------------------------------------------------------

#include "NeuralNet.h"
#include "Architecture.h"
#include "VarUtils.h"

//----------------------------------------------------------------------------
NeuralNet::NeuralNet(std::vector<int> structure, 
                     ann_type this_net, 
                     bool sse_err_func): 
        structure( structure ), 
        sse(sse_err_func),
        stddev(structure.at(0), 1.0), 
        mean(structure.at(0), 0.0) 
{
	learning = 0.1;
	momentum = 0.5;
	regress = ( this_net == regression );
	setActivationFunctions(sigmoid, dsig, softmax);
	Net = std::move(std::unique_ptr<Architecture>(new Architecture(structure, this_net, _sigmoid, _sigmoid_derivative)));	
	Net->setLearning(0.1);
	Net->setMomentum(0.5);
}
//----------------------------------------------------------------------------
NeuralNet::NeuralNet()
{
}
//----------------------------------------------------------------------------
NeuralNet::~NeuralNet() 
{
}
//----------------------------------------------------------------------------
NeuralNet::NeuralNet(NeuralNet &A) : Net(std::unique_ptr<Architecture>(new Architecture(A.Net->structure, A.Net->this_net, A.Net->_sigmoid_function, A.Net->_sigmoid_derivative))) 
{
	setActivationFunctions(A._sigmoid, A._sigmoid_derivative, A._softmax_function);
	for (unsigned int l = 0; l < Net->Bundle.size(); ++l) 
	{
		for (int i = 0; i <= Net->Bundle.at(l)->ins; ++i) 
		{
			for (int j = 0; j < Net->Bundle.at(l)->outs; ++j) 
			{
				Net->Bundle.at(l)->Synapse.at(i).at(j) = A.Net->Bundle.at(l)->Synapse.at(i).at(j);

				Net->Bundle.at(l)->DeltaSynapse.at(i).at(j) = A.Net->Bundle.at(l)->DeltaSynapse.at(i).at(j);
			}
		}
	}
	Net->setLearning(A.learning);
	Net->setMomentum(A.momentum);
}
//----------------------------------------------------------------------------
NeuralNet& NeuralNet::operator=(const NeuralNet &A)  
{
	if(this == &A)  
	{ 
		return *this;
	} 
	else  
	{
		Net = std::move(std::unique_ptr<Architecture>(new Architecture(A.Net->structure, A.Net->this_net,  A.Net->_sigmoid_function, A.Net->_sigmoid_derivative)));
		for (unsigned int l = 0; l < Net->Bundle.size(); ++l) 
		{
			for (int i = 0; i <= Net->Bundle.at(l)->ins; ++i) 
			{
				for (int j = 0; j < Net->Bundle.at(l)->outs; ++j) 
				{
					Net->Bundle.at(l)->Synapse.at(i).at(j) = A.Net->Bundle.at(l)->Synapse.at(i).at(j);
					Net->Bundle.at(l)->DeltaSynapse.at(i).at(j) = A.Net->Bundle.at(l)->DeltaSynapse.at(i).at(j);
				}
			}
		}
		Net->setLearning(A.learning);
		Net->setMomentum(A.momentum);
		return *this;
	}
}
//----------------------------------------------------------------------------
void NeuralNet::set_dataset(std::string root_file, std::string tree_name)

{
	dataset = std::move(std::unique_ptr<Dataset>(new Dataset(root_file, tree_name)));
}
//----------------------------------------------------------------------------
bool NeuralNet::set_input_branch(std::string name, std::string type)
{
	dataset->set_input_branch(name, type);
}
//----------------------------------------------------------------------------
bool NeuralNet::set_output_branch(std::string name, std::string type)
{
	dataset->set_output_branch(name, type);
}
//----------------------------------------------------------------------------
bool NeuralNet::set_control_branch(std::string name, std::string type)
{
	dataset->set_control_branch(name, type);
}
//----------------------------------------------------------------------------
double NeuralNet::get_value(std::string name)
{
	return dataset->get_value(name);
}
void NeuralNet::get_dataset_entry(const int index)
{
	dataset->at(index);
}
//----------------------------------------------------------------------------
double NeuralNet::get_physics_reweighting()
{
	return dataset->get_physics_reweighting();
}
//----------------------------------------------------------------------------
std::vector<double> NeuralNet::input()
{
	return dataset->input();
}
//----------------------------------------------------------------------------
std::vector<double> NeuralNet::output()
{
	return dataset->output();
}
//----------------------------------------------------------------------------
void NeuralNet::setLearning(double x) 
{
	learning = x;
	Net->setLearning(x);
}
//----------------------------------------------------------------------------
void NeuralNet::setMomentum(double x) 
{
	momentum = x;
	Net->setMomentum(x);
}
//----------------------------------------------------------------------------
void NeuralNet::anneal(double x) 
{
	Net->anneal(x);
}
//----------------------------------------------------------------------------
void NeuralNet::setActivationFunctions(std::vector<double> (*sigmoid_function) (std::vector<double>),\
                            double (*sigmoid_derivative)(double), \
                            std::vector<double> (*softmax_function) (std::vector<double>))
{
	_sigmoid_derivative = sigmoid_derivative;
	_softmax_function = softmax_function;
	_sigmoid = sigmoid_function;
}
//----------------------------------------------------------------------------
void NeuralNet::train(int n_epochs, int n_train, std::string save_filename, bool verbose, std::string timestamp)
{
	dataset->determine_reweighting();
	double pct;
	for (int i = 0; i < n_epochs; ++i) 
    {
    	save(".temp_progress_" + save_filename + std::to_string(i) + "_"+ timestamp + ".nnet"); //save a progress file in case we need to kill the process.
        for (int entry = 0; entry < n_train; entry++) 
        {
        	get_dataset_entry(entry);
        	// std::cout << "pT = " << get_value("pt") << ", and eta = " << get_value("eta") << std::endl;
            // if ((get_value("pt") < 500) && (fabs(get_value("eta")) < 2.5))
            if ((get_value("pt") < 500) && (get_value("pt") > 25) && (fabs(get_value("eta")) < 2.5))
            {
            	// std::cout << "And we train..." << std::endl;
            	get_dataset_entry(entry);
            	train(input(), output(), get_physics_reweighting());
            }

            pct = (((double)(entry)) / ((double) (n_train))) * 100;
            if (verbose)
            {
                epoch_progress_bar(pct, i + 1, n_epochs);
            }
        }
        
    }
    if (verbose)
    {
    	std::cout << "Saving parameters to " << save_filename << "." << std::endl; 
    }
    save(save_filename);
}
//----------------------------------------------------------------------------
void NeuralNet::train(std::vector<double> Event, std::vector<double> Actual, double weight) 
{
	std::vector<double> outs((Net->test( transform(Event) )));
	assert ( outs.size() == Actual.size());
	if (!regress) 
	{
		outs = _softmax_function(outs);


		if(!sse) 
		{
			for (unsigned int i = 0; i < outs.size(); ++i) 
			{
				outs.at(i) -= Actual.at(i);     // what i was doing before
				outs.at(i) /= log(2);

				// outs.at(i) -= Actual.at(i);
				// outs.at(i) /= log(2);
				// outs.at(i) *= Actual.at(i);
				// outs.at(i) = _sigmoid_derivative(outs.at(i)) * (outs.at(i) - Actual.at(i));
			}
		}
		else 
		{
			for (unsigned int i = 0; i < outs.size(); ++i) 
			{
				// outs.at(i) -= Actual.at(i);
				outs.at(i) = _sigmoid_derivative(outs.at(i)) * (outs.at(i) - Actual.at(i));
			}
		}
	}
	else 
	{
		for (unsigned int i = 0; i < outs.size(); ++i) 
		{
			outs.at(i) = _sigmoid_derivative(outs.at(i)) * (outs.at(i) - Actual.at(i));
		}
	}

	Net->backpropagate(outs, transform(Event), weight);
}
//----------------------------------------------------------------------------
std::vector<double> NeuralNet::predict(std::vector<double> Event) 
{
	if (regress) 
	{
		return std::move(Net->test( transform(Event) ));
	}
	else 
	{ 
		return std::move(_softmax_function(Net->test( transform(Event) )));
	}
}
//----------------------------------------------------------------------------
void NeuralNet::getTransform(bool verbose, bool into_memory) 
{
	int n_estimate = (dataset->num_entries() / 20);
	unsigned int n = 0;
	double pct, temp;    
	if (verbose)
	{
		std::cout << "\nNormalizing input variables:\n";
	}
	dataset->at(0);
	std::vector<double> ENTRY(dataset->input());
    int n_cols = ENTRY.size();
    std::vector<double> means(n_cols, 0);
    std::vector<double> stdev(n_cols, 0);

	for (int i = 0; i < n_estimate; ++i)
	{
		dataset->at(i);
		if ((get_value("pt") > 25) && (fabs(get_value("eta")) < 2.5))
		{
			++n;
		    ENTRY = dataset->input();
		    if (into_memory)
		    {
		    	dataset_mem.push_back(ENTRY);
		    	weights_mem.push_back(get_physics_reweighting());
		    }
		     // online, numerically stable algorithm 
		    for (unsigned int j = 0; j < n_cols; ++j)
		    {
		        temp = ENTRY[j];
		        double old_mean = means[j];
		        means[j] += ((temp - means[j]) / n);
		        stdev[j] = ((n - 2) * stdev[j] + (temp - means[j]) * (temp - old_mean)) / std::max((double)(n - 1), 1.0000);

		    }
		}
		if (verbose)
		{
		    pct = (((double)(i)) / ((double) (n_estimate))) * 100;
		    progress_bar(pct);
		}
	}
	setTransform(means, stdev);
}
//----------------------------------------------------------------------------
void NeuralNet::setTransform(std::vector<double> Mean, std::vector<double> Stddev) 
{
	mean = Mean;
	stddev = Stddev;
}
//----------------------------------------------------------------------------
std::vector<double> NeuralNet::transform(std::vector<double> Event) 
{
	for (unsigned int i = 0; i < Event.size(); ++i) 
	{
		Event.at(i) -= mean.at(i);
		Event.at(i) /= ((stddev.at(i) < 1e-7) ? 1e-4 : stddev.at(i)); // avoid dirac delta-like variances
	}	
	return std::move(Event);
}
//----------------------------------------------------------------------------
std::vector<std::vector<double>> NeuralNet::transform(std::vector<std::vector<double>> Event) 
{
	for (auto &entry : Event)
	{
		for (unsigned int i = 0; i < entry.size(); ++i) 
		{
			entry.at(i) -= mean.at(i);
			entry.at(i) /= ((stddev.at(i) < 1e-7) ? 1e-4 : stddev.at(i)); // avoid dirac delta-like variances
		}	
	}
	
	return std::move(Event);
}
//----------------------------------------------------------------------------
void NeuralNet::encode(std::vector<std::vector<double>> input, std::vector<double> weight, bool verbose)
{
	Net->encode(transform(input), .007, weight, verbose);
}
//----------------------------------------------------------------------------
void NeuralNet::encode(bool verbose)
{
	if (dataset_mem.size() == 0)
	{
		std::cout << "Warning: skipping encoding, dataset not found in RAM." << std::endl;
	}
	else
	{
		encode(dataset_mem, weights_mem, verbose);
	}
}
//----------------------------------------------------------------------------
bool NeuralNet::save(const std::string &filename) 
{
    std::ofstream net_file( filename );
    if (!net_file.is_open()) 
    {
        std::cout << "\nError: File name " << filename << " invalid." << std::endl;
        return 0;
    }
    net_file << "#->NNET\n";
    if (regress) 
    {
    	net_file << 1;
    }
    else 
    {
    	net_file << 0;
    }
    for (unsigned int i = 0; i < structure.size(); ++i) 
    {
    	net_file << ", " << structure.at(i);
    }
    net_file << "\n" << learning << "?" << momentum << "\n";
    for (unsigned int l = 0; l < Net->Bundle.size(); ++l) 
    {
		net_file << "BUNDLE\n";
		for (int i = 0; i <= Net->Bundle.at(l)->ins; ++i) 
		{
			for (int j = 0; j < (Net->Bundle.at(l)->outs - 1); ++j) 
			{
				net_file << std::setprecision(11) << Net->Bundle.at(l)->Synapse.at(i).at(j) << ", ";
			}
			net_file << std::setprecision(11) << Net->Bundle.at(l)->Synapse.at(i).at((Net->Bundle.at(l)->outs - 1)) << "\n";
		}
	}
    net_file << "TRANS\n";
    for (unsigned int j = 0; j < (mean.size() - 1); ++j) 
    {
        net_file << std::setprecision(11) << mean.at(j) << ", ";
    }
    net_file << mean.at(mean.size() - 1) << "\n";

    for (unsigned int j = 0; j < (stddev.size() - 1); ++j) 
    {
        net_file << std::setprecision(11) << stddev.at(j) << ", ";
    }
    net_file << stddev.at(stddev.size() - 1) << "\n";

	net_file.close();
	return 1;
}
//----------------------------------------------------------------------------
bool NeuralNet::load(const std::string &filename) 
{
    std::string s;
    std::ifstream net_file( filename );
    if (!net_file.is_open()) 
    {
        std::cout << "\nError: File name " << filename << " not found." << std::endl;
        return 0;
    }
    std::getline( net_file, s );
    if ((s != "#->NNET")) 
    {
    	std::cout << "\nError: file type not recognised." << std::endl;
    	return 0;
    }
    std::getline( net_file, s );
    std::istringstream iss( s );
    std::vector<unsigned int> params;
    while (std::getline(iss, s, ',')) 
    {
        unsigned int fieldvalue = 0u;
        std::istringstream( s ) >> fieldvalue;
        params.push_back( fieldvalue );
    }
    std::vector<int> layer_struct;
    for (unsigned int i = 1; i < params.size(); ++i) 
    {
    	layer_struct.push_back(params.at(i));
    }

    ann_type new_net = ((params.at(0) == 1) ? regression : classification);
    Net = std::move(std::unique_ptr<Architecture>(new Architecture(layer_struct, new_net, sigmoid, dsig)));
    setActivationFunctions(sigmoid, dsig, softmax);

    std::getline( net_file, s );
    std::istringstream n_iss( s );
    params.clear();
    while (std::getline(n_iss, s, '?')) 
    {
        unsigned int fieldvalue = 0.00;
        std::istringstream( s ) >> fieldvalue;
        params.push_back( fieldvalue );
    }

    Net->setLearning(params.at(0));
    Net->setMomentum(params.at(1));
    bool synapse_phase = 0;
    int row_count = -1;
    int layer_count = -1;
    int trans_count = 0;
    while (std::getline( net_file, s )) 
    {
    	if (s == "BUNDLE") 
    	{
    		synapse_phase = 1;
    		layer_count++;
    		row_count = -1;
    	}
    	else if (s == "TRANS") 
    	{
    		synapse_phase = 0;
    	}
    	else if (synapse_phase) 
    	{
    		row_count++;
    		std::vector<double> record;
	        std::istringstream iss( s );
	        while (std::getline( iss, s, ',' )) 
	        {
	            double fieldvalue = 0.0;
	            std::istringstream( s ) >> fieldvalue;
	            record.push_back( fieldvalue );
	        }
	        for (unsigned int i = 0; i < record.size(); ++i) 
	        {
	        	(Net->Bundle.at(layer_count))->Synapse.at(row_count).at(i) = record.at(i);
	        }
    	}
		

    	else if ((!synapse_phase) & (trans_count == 0)) 
    	{
    		trans_count++;
    		std::vector<double> record;
	        std::istringstream iss( s );
	        while (std::getline( iss, s, ',' )) 
	        {
	            double fieldvalue = 0.0;
	            std::istringstream( s ) >> fieldvalue;
	            record.push_back( fieldvalue );
	        }
	        mean.resize(record.size());
	        for (unsigned int i = 0; i < record.size(); ++i) 
	        {
	        	mean.at(i) = record.at(i);
	        }
    	}
    	else if ((!synapse_phase) & (trans_count == 1)) 
    	{
    		trans_count++;
    		std::vector<double> record;
	        std::istringstream iss( s );
	        while (std::getline( iss, s, ',' )) 
	        {
	            double fieldvalue = 0.0;
	            std::istringstream( s ) >> fieldvalue;

	            record.push_back( fieldvalue );
	        }
	        stddev.resize(record.size());
			

	        for (unsigned int i = 0; i < record.size(); ++i) 
	        {
	        	stddev.at(i) = record.at(i);
	        }

    	}
    }
    return net_file.good();
}
bool NeuralNet::load_specifications(const std::string &filename)
{
	std::string line;
    std::ifstream FILE( filename );
    bool input_phase = false, output_phase = false, control_phase = false;
    if (!FILE.is_open()) 
    {
        std::cout << "\nError: Specification file name " << filename << " not found." << std::endl;
        return 0;
    }
    while(std::getline( FILE, line ))
    {
    	line = trim(line);
    	if (line == "input:")
    	{
    		input_phase = true;
    		output_phase = false;
    		control_phase = false;
    	}
    	if (line == "output:")
    	{
    		output_phase = true;
    		input_phase = false;
    		control_phase = false;
    	}
    	if (line == "control:")
    	{
    		control_phase = true;
    		input_phase = false;
    		output_phase = false;
    	}
    	if (line[0] != '#')
    	{
    		if (input_phase)
	    	{
	    		std::string name, type;
	    		std::istringstream field( line );
	    		std::getline(field, name, ',');
	    		std::getline(field, type, ',');
	    		if ((name != "") && (type != ""))
	    		{
	    			set_input_branch(trim(name), trim(type));
	    		}
	    	}
	    	else if (output_phase)
	    	{
	    		std::string name, type;
	    		std::istringstream field( line );
	    		std::getline(field, name, ',');
	    		std::getline(field, type, ',');
	    		if ((name != "") && (type != ""))
	    		{
	    			set_output_branch(trim(name), trim(type));
	    		}
	    	}
	    	else if (control_phase)
	    	{
	    		std::string name, type;
	    		std::istringstream field( line );
	    		std::getline(field, name, ',');
	    		std::getline(field, type, ',');
	    		if ((name != "") && (type != ""))
	    		{
	    			set_control_branch(trim(name), trim(type));
	    		}
	    	}
    	}
    	else
    	{
    		std::cout << "Error: Specification file format not recognized." << std::endl;
    		return false;
    	}
    }
    return FILE.good();
}


