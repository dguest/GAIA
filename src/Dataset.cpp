#include "Dataset.h"

#include <cmath>

struct Numeric;
struct Reweighting;

//----------------------------------------------------------------------------

Dataset::Dataset(std::string root_file, std::string tree_name) : fail( false )
{
	if (root_file.empty())
	{
		std::cout << "Error: A ROOT file must be specified." << std::endl;
		fail = true;
	}
	if (tree_name.empty())
	{
		std::cout << "Error: A TTree name must be specified." << std::endl;
		fail = true;
	}
	if (!fail)
	{
		file = new TFile(root_file.c_str());
		tree = static_cast<TTree*> (file->Get(tree_name.c_str()));
		m_input.reserve(0);
		m_output.reserve(0);
		n_entries = tree->GetEntries();
	}
	else
	{
		this->~Dataset();
	}
}

//----------------------------------------------------------------------------

Dataset::~Dataset()
{
	if (!fail)
	{
		delete file;
	}
}
//----------------------------------------------------------------------------
std::vector<std::string> Dataset::get_output_vars()
{
	return output_vars;
}
//----------------------------------------------------------------------------
std::vector<std::string> Dataset::get_input_vars()
{
	return input_vars;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool Dataset::set_input_branch(std::string name, std::string type)
{
	variables.insert(std::pair<std::string, std::unique_ptr<Numeric>>(name, std::move(std::unique_ptr<Numeric>(new Numeric))));
	if (type == "double")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->double_);
		variables[name]->isDbl = true;
		input_vars.push_back(name);
		m_input.resize(input_vars.size());
		return 1;
	}
	else if (type == "float")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->float_);
		variables[name]->isFlt = true;
		input_vars.push_back(name);
		m_input.resize(input_vars.size());
		return 1;
	}
	else if (type == "int")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->int_);
		variables[name]->isInt = true;
		input_vars.push_back(name);
		m_input.resize(input_vars.size());
		return 1;
	}
	else
	{
		std::cout << "Error: type \"" << type << "\" not recognized." << std::endl;
		variables.erase(name);
		return 0;
	}
}
//----------------------------------------------------------------------------
bool Dataset::set_output_branch(std::string name, std::string type)
{
	variables.insert(std::pair<std::string, std::unique_ptr<Numeric>>(name, std::move(std::unique_ptr<Numeric>(new Numeric))));
	if (type == "double")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->double_);
		variables[name]->isDbl = true;
		output_vars.push_back(name);
		m_output.resize(output_vars.size());
		return 1;
	}
	else if (type == "float")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->float_);
		variables[name]->isFlt = true;
		output_vars.push_back(name);
		m_output.resize(output_vars.size());
		return 1;
	}
	else if (type == "int")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->int_);
		variables[name]->isInt = true;
		output_vars.push_back(name);
		m_output.resize(output_vars.size());
		return 1;
	}
	else
	{
		std::cout << "Error: type \"" << type << "\" not recognized." << std::endl;
		return 0;
	}
}

//----------------------------------------------------------------------------
bool Dataset::set_control_branch(std::string name, std::string type)
{
	variables.insert(std::pair<std::string, std::unique_ptr<Numeric>>(name, std::move(std::unique_ptr<Numeric>(new Numeric))));
	if (type == "double")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->double_);
		variables[name]->isDbl = true;
		control_vars.push_back(name);
		return 1;
	}
	else if (type == "float")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->float_);
		variables[name]->isFlt = true;
		control_vars.push_back(name);
		return 1;
	}
	else if (type == "int")
	{
		tree->SetBranchAddress(name.c_str(), &variables[name]->int_);
		variables[name]->isInt = true;
		control_vars.push_back(name);
		return 1;
	}
	else
	{
		std::cout << "Error: type \"" << type << "\" not recognized." << std::endl;
		return 0;
	}
}

//----------------------------------------------------------------------------

std::map<std::string, double> Dataset::get_performance_map(std::vector<std::string> &variable_names)
{
	std::map<std::string, double> temp;
	for (auto &name : variable_names)
	{
		temp[name] = cast_as_double(*(variables[name]));
	}
	return std::move(temp);
}


//----------------------------------------------------------------------------

void Dataset::operator[]( const int index )
{
	tree->GetEntry(index);
}

void Dataset::at( const int index )
{
	tree->GetEntry(index);
}
//----------------------------------------------------------------------------

double Dataset::get_value(std::string name)
{
	return cast_as_double(*(variables[name]));
}
//----------------------------------------------------------------------------
std::vector<double> &Dataset::input()
{
	unsigned int ptr = 0;

	for (auto name : input_vars)
	{
		m_input[ptr] = cast_as_double(*(variables[name]));
		++ptr;
	}

	return m_input;
}
//----------------------------------------------------------------------------
std::vector<double> &Dataset::output()
{
	unsigned int ptr = 0;
	for (auto name : output_vars)
	{
		m_output[ptr] = cast_as_double(*(variables[name]));
		++ptr;
	}
	return m_output;
}
//----------------------------------------------------------------------------
double Dataset::get_physics_reweighting()
{
	if (cast_as_int(*(variables["light"])) == 1)
    {
        return 1.0;
    }
    else if (cast_as_int(*(variables["bottom"])) == 1)
    {
        return reweighting.bottom_correction[cast_as_int(*(variables["cat_pT"]))][cast_as_int(*(variables["cat_eta"]))];
    }
    else
    {
        return reweighting.charm_correction[cast_as_int(*(variables["cat_pT"]))][cast_as_int(*(variables["cat_eta"]))];
    }
}
//----------------------------------------------------------------------------
unsigned int Dataset::num_entries()
{
	return n_entries;
}
//----------------------------------------------------------------------------
void Dataset::determine_reweighting()
{
	int n_estimate = n_entries / 20;
	std::vector<std::vector<double> > charm_correction, bottom_correction, charm_hist, bottom_hist, light_hist;
	charm_correction.resize(7);
	bottom_correction.resize(7);
	charm_hist.resize(7);
	bottom_hist.resize(7);
	light_hist.resize(7);
	for (int i = 0; i < 7; ++i)
	{
		charm_correction[i].resize(4);
		bottom_correction[i].resize(4);
		charm_hist[i].resize(4);
		bottom_hist[i].resize(4);
		light_hist[i].resize(4);
	}
	for (int cat_pT = 0; cat_pT < 7; ++cat_pT)
	{
		for (int cat_eta = 0; cat_eta < 4; ++cat_eta)
		{
			light_hist[cat_pT][cat_eta] = 0;
			charm_hist[cat_pT][cat_eta] = 0;
			bottom_hist[cat_pT][cat_eta] = 0;
		}
	}
	for (int i = 0; i < n_estimate; ++i)
	{
		at(i);
		if ((fabs(get_value("eta")) < 2.5) && (get_value("pt") > 20) && (get_value("flavor_truth_label") < 8) && (get_value("pt") < 1000))
		{
			if (cast_as_int(*variables["light"]) == 1)
			{
				light_hist[cast_as_int(*(variables["cat_pT"]))][cast_as_int(*(variables["cat_eta"]))] += 1;
			}
			else if (cast_as_int(*variables["charm"]) == 1)
			{
				charm_hist[cast_as_int(*(variables["cat_pT"]))][cast_as_int(*(variables["cat_eta"]))] += 1;
			}
			else if (cast_as_int(*variables["bottom"]) == 1)
			{
				bottom_hist[cast_as_int(*(variables["cat_pT"]))][cast_as_int(*(variables["cat_eta"]))] += 1;
			}
		}

	}
	for (int cat_pT = 0; cat_pT < 7; ++cat_pT)
	{
		for (int cat_eta = 0; cat_eta < 4; ++cat_eta)
		{
			bottom_correction[cat_pT][cat_eta] = std::min(std::max(light_hist[cat_pT][cat_eta], 1.0) / ((5.17 / 4.0) * std::max(bottom_hist[cat_pT][cat_eta], 1.0)), 20.0);
			charm_correction[cat_pT][cat_eta] = std::min(std::max(light_hist[cat_pT][cat_eta], 1.0) / (2.5 * std::max(charm_hist[cat_pT][cat_eta], 1.0)), 20.0);
		}
	}
	reweighting.charm_correction = charm_correction;
	reweighting.bottom_correction = bottom_correction;
}

//----------------------------------------------------------------------------
//------------------ NON CLASS UTILITY-TYPE FUNCTIONS ------------------------
//----------------------------------------------------------------------------


inline double cast_as_double(Numeric number)
{
	if (number.isInt)
	{
		return static_cast<double>(number.int_);
	}
	else if (number.isFlt)
	{
		return static_cast<double>(number.float_);
	}
	else if (number.isDbl)
	{
		return static_cast<double>(number.double_);
	}
}
//----------------------------------------------------------------------------
inline int cast_as_int(Numeric number)
{
	if (number.isInt)
	{
		return static_cast<int>(number.int_);
	}
	else if (number.isFlt)
	{
		return static_cast<int>(number.float_);
	}
	else if (number.isDbl)
	{
		return static_cast<int>(number.double_);
	}
}


