//------------------------------------------------------
//				NeuralNet.cpp
//				By: Luke de Oliveira
//------------------------------------------------------

#include "NeuralNet.h"
#include "Architecture.h"
#include <utility>


//----------------------------------------------------------------------------
NeuralNet::NeuralNet(std::vector<int> structure): 
        structure( structure ), 
        stddev(structure.at(0), 1.0), 
        mean(structure.at(0), 0.0) 
{
	learning = 0.1;
	momentum = 0.5;
	setActivationFunctions(sigmoid, dsig, softmax);
	Net = std::move(std::unique_ptr<Architecture>(new Architecture(structure, _sigmoid, _sigmoid_derivative)));	
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
NeuralNet::NeuralNet(NeuralNet &A) : Net(std::unique_ptr<Architecture>(new Architecture(A.Net->structure, A.Net->_sigmoid_function, A.Net->_sigmoid_derivative))) 
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
		Net = std::move(std::unique_ptr<Architecture>(new Architecture(A.Net->structure, A.Net->_sigmoid_function, A.Net->_sigmoid_derivative)));
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
void NeuralNet::setActivationFunctions(std::vector<double> (*sigmoid_function) (std::vector<double>),
                                       double (*sigmoid_derivative)(double), 
                                       std::vector<double> (*softmax_function) (std::vector<double>))
{
	_sigmoid_derivative = sigmoid_derivative;
	_softmax_function = softmax_function;
	_sigmoid = sigmoid_function;
}
//----------------------------------------------------------------------------
void NeuralNet::train(int n_epochs, int n_train, std::string save_filename, bool verbose, std::string timestamp, bool memory)
{
	double pct;
	if(!memory)
	{
		for (int i = 0; i < n_epochs; ++i) 
	    {
	    	save(".temp_progress_" + save_filename + std::to_string(i) + "_"+ timestamp + ".nnet"); //save a progress file in case we need to kill the process.
	        for (int entry = 0; entry < n_train; entry++) 
	        {
	        	get_dataset_entry(entry);

	        	bool good_jet = (get_value("pt") > 20) && 
	            	            (fabs(get_value("eta")) < 2.5) && 
	            	            (get_value("flavor_truth_label") < 8) && 
	            	            (get_value("pt") < 1000);

	            if ( good_jet )
	            {
	            	// Net->weight_dropout();
	        		train(input(), output(), get_physics_reweighting());
	            }
	            pct = (((double)(entry)) / ((double) (n_train))) * 100;
	            if (verbose)
	            {
	                epoch_progress_bar(pct, i + 1, n_epochs);
	            }
	        }	        
	    }
	}
	else
	{
		int n = weights_mem.size();
		for (int i = 0; i < n_epochs; ++i) 
	    {
	    	//save a progress file in case we need to kill the process.
	    	save(".temp_progress_" + save_filename + std::to_string(i) + "_"+ timestamp + ".nnet"); 
	        for (int entry = 0; entry < n; entry++) 
	        {
	        	train(dataset_mem.at(entry), labels_mem.at(entry), weights_mem.at(entry));
	            pct = (((double)(entry)) / ((double) (n))) * 100;
	            if (verbose)
	            {
	                epoch_progress_bar(pct, i + 1, n_epochs);
	            }
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
	outs = _softmax_function(outs);
	for (unsigned int i = 0; i < outs.size(); ++i) 
	{
		outs.at(i) -= Actual.at(i);
		outs.at(i) /= log(2);
	}
	Net->backpropagate(outs, transform(Event), weight);
}
//----------------------------------------------------------------------------
std::vector<double> NeuralNet::predict(std::vector<double> Event) 
{
	return std::move(_softmax_function(Net->test( transform(Event) )));
}
//----------------------------------------------------------------------------
void NeuralNet::getTransform(bool verbose, bool into_memory, int n_train) 
{
	dataset->determine_reweighting();
	int n_estimate = ((n_train < 0) ? (dataset->num_entries() / 20) : n_train);
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
		if ((get_value("pt") > 20) && (fabs(get_value("eta"))) < 2.5 && (get_value("flavor_truth_label") < 8) && (get_value("pt") < 1000))
		{
			++n;
		    ENTRY = dataset->input();
		    if (into_memory)
		    {
		    	auto labels = dataset->output();
		    	dataset_mem.push_back(ENTRY);
		    	labels_mem.push_back(labels);
		    	weights_mem.push_back(get_physics_reweighting());
		    }
		    // online, numerically stable algorithm 
		    for (unsigned int j = 0; j < n_cols; ++j)
		    {
		        temp = ENTRY[j];
		        double old_mean = means[j];
		        means[j] += ((temp - means[j]) / n);
		        double denom = ((n == 1) ? 1.0 : ((double)n - 1));
		        stdev[j] = ((n - 2) * stdev[j] + (temp - means[j]) * (temp - old_mean)) / denom;
		    }
		}
		if (verbose)
		{
		    pct = (((double)(i)) / ((double) (n_estimate))) * 100;
		    progress_bar(pct);
		}
	}
	for (unsigned int j = 0; j < n_cols; ++j) 
    {
        stdev[j] = sqrt(stdev[j]);
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
	net_file << 0;
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

    Net = std::move(std::unique_ptr<Architecture>(new Architecture(layer_struct, sigmoid, dsig)));
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
bool NeuralNet::write_perf( const std::string &filename, int start, int end)
{
	std::vector<std::string> perf_variables {"cat_pT",
                                             "cat_eta",
                                             "pt",
                                             "eta",
                                             "MV1",
                                             "jfitc_pu",
                                             "jfitc_pb",
                                             "jfitc_pc",
                                             "jfitcomb_pu",
                                             "jfitcomb_pb",
                                             "jfitcomb_pc",
                                             "bottom",
                                             "charm",
                                             "light"};
	if (filename.empty())
	{
		std::cout << std::endl;
		auto output_variables = dataset->get_output_vars();
		int ptr = 0;
		for (auto &name : output_variables)
		{	
			if (ptr != 0)
			{
				std::cout << ", ";
			}
			std::cout << "prob_" << name;
			++ptr;
		}

        for (auto &name : perf_variables)
        {
        	std::cout << ", " << name;
        }
        std::cout << std::endl;
        for (int entry = start; entry < end; ++entry)
        {
        	get_dataset_entry(entry);
        	if ((get_value("pt") > 20) && 
        		(get_value("pt") < 10000) && 
        		(get_value("flavor_truth_label") < 8))
        	{
        		ptr = 0;
        		std::vector<double> predicted_values(_softmax_function(Net->test(transform(input() ))));
        		for (auto &prob : predicted_values)
        		{
        			if (ptr != 0)
					{
						std::cout << ", ";
					}
        			std::cout << prob;
        			ptr++;
        		}
        		auto perf_values = dataset->get_performance_map(perf_variables);
        		for (auto &name : perf_variables)
        		{
        			std::cout << ", " << perf_values[name];
        		}
        		std::cout << "\n";
        	}
        }
    }
    else
    {
    	ofstream file( filename );
    	if(file.is_open())
    	{
			auto output_variables = dataset->get_output_vars();
			int ptr = 0;
			for (auto &name : output_variables)
			{	
				if (ptr != 0)
				{
					file << ", ";
				}
				file << "prob_" << name;
				++ptr;
			}

	        for (auto &name : perf_variables)
	        {
	        	file << ", " << name;
	        }
	        file << std::endl;
	        for (int entry = start; entry < end; ++entry)
	        {
	        	get_dataset_entry(entry);
	        	if ((get_value("pt") > 20) && 
	        		(get_value("pt") < 10000) && 
	        		(get_value("flavor_truth_label") < 8))
	        	{
	        		ptr = 0;
	        		std::vector<double> predicted_values(_softmax_function(Net->test(transform(input() ))));
	        		for (auto &prob : predicted_values)
	        		{
	        			if (ptr != 0)
						{
							file << ", ";
						}
	        			file << prob;
	        			ptr++;
	        		}
	        		auto perf_values = dataset->get_performance_map(perf_variables);
	        		for (auto &name : perf_variables)
	        		{
	        			file << ", " << perf_values[name];
	        		}
	        		file << "\n";
	        	}
	        }
	        file.close();
	    }
    }
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


std::vector<std::string> NeuralNet::get_ranking()
{
	auto variable_list = dataset->get_input_vars();
	unsigned int num_vars = variable_list.size();
	std::vector<DictElement> contribution;;
	auto First_Layer = Net->get_first_layer();
	std::vector<std::string> ranked;
	for (int i = 0; i < num_vars; ++i)
	{
		contribution.push_back(std::make_pair(variable_list.at(i), (mean.at(i) * mean.at(i)) * get_row_sum(First_Layer, i)));
	}

	_ranking_comparison comparison_operator;

	std::sort(contribution.begin(), contribution.end(), comparison_operator);
	for (auto &entry : contribution)
	{
		ranked.push_back(entry.first);
	}
	return ranked;
}


