#include "Tagger.h"

struct Specification;
// struct Event;

//----------------------------------------------------------------------------

Tagger::Tagger(Specification specs) : fail( false )
{
	if (specs.root_file.empty())
	{
		std::cout << "Error: A ROOT file must be specified." << std::endl;
		fail = true;
	}
	if (specs.tree_name.empty())
	{
		std::cout << "Error: A TTree name must be specified." << std::endl;
		fail = true;
	}
	if (specs.load_file.empty())
	{
		std::cout << "Error: A .nnet file must be specified." << std::endl;
		fail = true;
	}
	if (!fail)
	{
		file = new TFile(root_file.c_str());
		tree = new TTree(file->Get(tree_name.c_str()));
		// net.load(specs.load_file);
		n_entries = tree->GetEntries();
		if ((specs.train + specs.test) > n_entries)
		{
			specs.test -= (specs.train + specs.test - n_entries)
		}
		test = specs.test;
		train = specs.train;

		for (auto entry = specs.type.begin(); entry != specs.type.end(); ++entry)
		{
			Numeric v;
			std::string this_type = entry->second;
			
			if (this_type == "float")
			{
				v = (float) 0.0;
			}
			if (this_type == "int")
			{
				v = (int) 0;
			}

			variables.insert(std::pair<std::string, Numeric>(entry->first, v));
		}
		for (auto &var = variables.begin(); var != variables.end(); ++var)
		{
			tree->SetBranchAddress(var->first.c_str(), &var->second);
		}
	}
}

//----------------------------------------------------------------------------

Tagger::~Tagger()
{
	if (!fail)
	{
		delete file;
		delete tree;
	}
}

//----------------------------------------------------------------------------

void Tagger::test(/*unsigned from, unsigned to, std::vector<std::vector<double>> &perf_data*/)
{
	tree->GetEntry(0)
	for (auto &var = variables.begin(); var != variables.end(); ++var)
	{
		std::cout << var->second << std::endl;
	}
}





