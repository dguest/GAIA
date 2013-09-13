//------------------------------------------------------
//				JetTagger.h
//				By: Luke de Oliveira
//------------------------------------------------------

#ifndef JETTAGGER_H
#define JETTAGGER_H 

#include <map>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <utility>
#include <string>
#include <cmath>

//-----------------------------------------------------------------------------
//	FORWARD DECLARATIONS OF FUNCTIONS AND UTILITY STRUCTURES
//-----------------------------------------------------------------------------

typedef std::pair<std::string, double> DictElement;


//make a namespace for safety
namespace JetTagger
{
	inline double sig(double x) ;
	inline void vector_print(std::vector<double> v);
	inline double dsig(double x) ;
	inline std::vector<double> sigmoid(std::vector<double> A) ;
	inline std::vector<double> dsigmoid(std::vector<double> A) ;
	inline std::vector<double> softmax(std::vector<double> A) ;
	inline std::string trim(const std::string& str, const std::string& whitespace = " ");


	class Layer
	{
	public:
	//----------------------------------------------------------------------------
		Layer(int ins, int outs, bool last, std::vector<double> (*Activation_function)(std::vector<double>));
		Layer(std::vector<std::vector<double> > Synapse, bool last);
		~Layer();
		std::vector<double> fire();
		void feed(std::vector<double> event);

	private:
	//----------------------------------------------------------------------------
		friend class NetworkArchitecture;
		friend class NeuralNet;
		std::vector<std::vector<double> > Synapse;
		std::vector<double> Outs;
		std::vector<double> (*_sigmoid)(std::vector<double>);
		int ins, outs;
		bool last;
		double gamma, onemingamma;
	};

	//----------------------------------------------------------------------------
	Layer::Layer(int ins, int outs, bool last, 
		         std::vector<double> (*Activation_function)(std::vector<double>)): 
				 ins(ins), outs(outs), last(last), Outs(outs, 0.00), 
	              _sigmoid(Activation_function)

	{
		std::vector<double> new_vec(outs, 0.00);
		for (int i = 0; i < ins + 1; ++i) 
		{
			Synapse.push_back(new_vec);
		}
	}

	//----------------------------------------------------------------------------
	Layer::Layer(std::vector<std::vector<double> > Synapse, bool last) : Synapse ( Synapse ), Outs(outs, 0.00)
	{
	}

	//----------------------------------------------------------------------------
	Layer::~Layer() 
	{
	}

	//----------------------------------------------------------------------------
	inline std::vector<double> Layer::fire() 
	{
		return Outs;
	}


	//----------------------------------------------------------------------------
	inline void Layer::feed(std::vector<double> event) 
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
	class NetworkArchitecture
	{
	public:
	//----------------------------------------------------------------------------
		NetworkArchitecture(std::vector<int> structure, std::vector<double> (*sigmoid_function) (std::vector<double>), double (*sigmoid_derivative) (double)):
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
		~NetworkArchitecture();
		std::vector<double> test(std::vector<double> Event);
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

	//----------------------------------------------------------------------------
	NetworkArchitecture::~NetworkArchitecture() 
	{
		Bundle.clear();
	}

	//----------------------------------------------------------------------------
	inline std::vector<double> NetworkArchitecture::test(std::vector<double> Event) 
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
	inline std::vector<std::vector<double>> NetworkArchitecture::get_first_layer()
	{
		return Bundle.at(0)->Synapse;
	}


	class NeuralNet
	{
	public:
	//----------------------------------------------------------------------------
		NeuralNet( std::vector<int> structure);
		NeuralNet();
		~NeuralNet();
		NeuralNet( NeuralNet &A );

		bool load_specifications(const std::string &filename);


		void setActivationFunctions(std::vector<double> (*sigmoid_function) (std::vector<double>),
	                                double (*sigmoid_derivative)(double), 
	                                std::vector<double> (*softmax_function) (std::vector<double>));

		std::vector<double> transform( std::vector<double> Event );

		std::map<std::string, double> predict( std::map<std::string, double> Event );
		NeuralNet& operator=( const NeuralNet &A );
		bool load_net( const std::string &filename );
	private:
	//----------------------------------------------------------------------------
		std::unique_ptr<NetworkArchitecture> Net;
		std::vector<int> structure;
		std::vector<std::string> input_names, output_names;
		int count;
		std::vector<double> mean, stddev, input_vector;
		double (*_sigmoid_derivative) (double);
		std::vector<double> (*_softmax_function) (std::vector<double>);
		std::vector<double> (*_sigmoid) (std::vector<double>);
	};



	//----------------------------------------------------------------------------

	NeuralNet::NeuralNet(std::vector<int> structure): 
	        structure( structure ), 
	        stddev(structure.at(0), 1.0), 
	        mean(structure.at(0), 0.0) 
	{
		setActivationFunctions(sigmoid, dsig, softmax);
		Net = std::move(std::unique_ptr<NetworkArchitecture>(new NetworkArchitecture(structure, _sigmoid, _sigmoid_derivative)));	
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
	NeuralNet::NeuralNet(NeuralNet &A) : 
	Net(std::unique_ptr<NetworkArchitecture>(new NetworkArchitecture(A.Net->structure, 
													   A.Net->_sigmoid_function, 
													   A.Net->_sigmoid_derivative))) 
	{
		setActivationFunctions(A._sigmoid, A._sigmoid_derivative, A._softmax_function);
		for (unsigned int l = 0; l < Net->Bundle.size(); ++l) 
		{
			for (int i = 0; i <= Net->Bundle.at(l)->ins; ++i) 
			{
				for (int j = 0; j < Net->Bundle.at(l)->outs; ++j) 
				{
					Net->Bundle.at(l)->Synapse.at(i).at(j) = A.Net->Bundle.at(l)->Synapse.at(i).at(j);
				}
			}
		}
	}
	//----------------------------------------------------------------------------
	inline NeuralNet& NeuralNet::operator=(const NeuralNet &A)  
	{
		if(this == &A)  
		{ 
			return *this;
		} 
		else  
		{
			Net = std::move(std::unique_ptr<NetworkArchitecture>(new NetworkArchitecture(A.Net->structure, 
														                   A.Net->_sigmoid_function, 
														                   A.Net->_sigmoid_derivative)));
			for (unsigned int l = 0; l < Net->Bundle.size(); ++l) 
			{
				for (int i = 0; i <= Net->Bundle.at(l)->ins; ++i) 
				{
					for (int j = 0; j < Net->Bundle.at(l)->outs; ++j) 
					{
						Net->Bundle.at(l)->Synapse.at(i).at(j) = A.Net->Bundle.at(l)->Synapse.at(i).at(j);
					}
				}
			}
			return *this;
		}
	}
	//----------------------------------------------------------------------------
	inline void NeuralNet::setActivationFunctions(std::vector<double> (*sigmoid_function) (std::vector<double>),
	                                       double (*sigmoid_derivative)(double), 
	                                       std::vector<double> (*softmax_function) (std::vector<double>))
	{
		_sigmoid_derivative = sigmoid_derivative;
		_softmax_function = softmax_function;
		_sigmoid = sigmoid_function;
	}
	//----------------------------------------------------------------------------
	inline std::map<std::string, double> NeuralNet::predict(std::map<std::string, double> Event) 
	{
		int ptr = 0;
		for (auto &entry : input_names)
		{
			input_vector.at(ptr) = Event[entry];
			++ptr;
		}
		ptr = 0;

		auto predicted = std::move(_softmax_function(Net->test( transform(input_vector) )));

		std::map<std::string, double> outputs;

		for (auto &entry : predicted)
		{
			outputs[output_names[ptr]] = entry;
			ptr++;
		}
		return outputs;
	}
	//----------------------------------------------------------------------------
	inline std::vector<double> NeuralNet::transform(std::vector<double> Event) // work
	{
		for (unsigned int i = 0; i < mean.size(); ++i) 
		{
			Event.at(i) -= mean.at(i);
			Event.at(i) /= ((stddev.at(i) < 1e-7) ? 1e-4 : stddev.at(i)); // avoid dirac delta-like variances
		}	
		return std::move(Event);
	}
	//----------------------------------------------------------------------------
	inline bool NeuralNet::load_net(const std::string &filename) 
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

	    Net = std::move(std::unique_ptr<NetworkArchitecture>(new NetworkArchitecture(layer_struct, sigmoid, dsig)));
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

	inline bool NeuralNet::load_specifications(const std::string &filename)
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
		    			input_names.push_back(trim(name));
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
		    			output_names.push_back(trim(name));
		    		}
		    	}
	    	}
	    	else
	    	{
	    		std::cout << "Error: Specification file format not recognized." << std::endl;
	    		return false;
	    	}
	    }
	    input_vector.resize(input_names.size(), 0.0);
	    return FILE.good();
	}


	//-----------------------------------------------------------------------------
	//	Definitions of utility-type functions
	//-----------------------------------------------------------------------------



	//----------------------------------------------------------------------------
	inline double sig(double x) 
	{
		return 1 / (1 + exp(-x));
	}
	//----------------------------------------------------------------------------
	inline void vector_print(std::vector<double> v)
	{
	    for (auto i : v)
	    {
	        std::cout << i << ",  ";
	    }
	    std::cout << "\n";
	}
	//----------------------------------------------------------------------------
	inline double dsig(double x) 
	{
		return x * (1 - x);
	}

	//----------------------------------------------------------------------------
	inline std::vector<double> sigmoid(std::vector<double> A) 
	{
		for(auto &element : A) 
		{
			element = sig(element);
		}
		return std::move(A);
	}
	//----------------------------------------------------------------------------
	inline std::vector<double> dsigmoid(std::vector<double> A) 
	{
		for(auto &element : A) 
		{
			element = dsig(element);
		}
		return std::move(A);
	}
	//----------------------------------------------------------------------------
	inline std::vector<double> softmax(std::vector<double> A) 
	{
		double sum = 0;
		for(auto &element : A) 
		{
			element = exp(element);
			sum += element;
		}
		for(auto &element : A) 
		{
			element /= sum;
		}
		return std::move(A);
	}
	//----------------------------------------------------------------------------
	inline std::string trim(const std::string& str, const std::string& whitespace)
	{
	    const auto strBegin = str.find_first_not_of(whitespace);
	    if (strBegin == std::string::npos)
	    {
	    	return ""; // no content
	    }
	        
	    const auto strEnd = str.find_last_not_of(whitespace);
	    const auto strRange = strEnd - strBegin + 1;
	    return str.substr(strBegin, strRange);
	}
}


#endif


