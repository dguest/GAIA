#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <TFile.h>
#include <TTree.h>

struct Numeric;

struct Reweighting
{
	std::vector<std::vector<double> > charm_correction {0};
    std::vector<std::vector<double> > bottom_correction {0};
};

class Dataset
{
public:
	Dataset(std::string root_file = "", std::string tree_name = "");
	~Dataset();
	bool set_input_branch(std::string name, std::string type);
	bool set_output_branch(std::string name, std::string type);
	bool set_control_branch(std::string name, std::string type);
	std::map<std::string, double> get_performance_map(std::vector<std::string> &variable_names);
	double get_value(std::string name);
	void operator[]( const int index );
	void at( const int index );
	std::vector<double> &input();
	std::vector<double> &output();
	std::vector<std::string> get_output_vars();
	std::vector<std::string> get_input_vars();
	unsigned int num_entries();
	double get_physics_reweighting();
	void determine_reweighting();
	
private:
	TFile *file;
	TTree *tree;
	bool fail;
	std::map<std::string, std::unique_ptr<Numeric>> variables;
	unsigned int n_entries;
	std::vector<std::string> input_vars, output_vars, control_vars;
	std::vector<double> m_input, m_output;
	Reweighting reweighting;
};

//----------------------------------------------------------------------------
//------------------ NON CLASS UTILITY-TYPE FUNCTIONS ------------------------
//----------------------------------------------------------------------------

struct Numeric
{
	double double_;
	float  float_;
	int    int_;
	bool   isDbl = false, 
	       isInt = false,
	       isFlt = false; 
};

inline double cast_as_double(Numeric number);
inline int cast_as_int(Numeric number);




