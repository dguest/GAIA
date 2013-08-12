#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <utility>
#include <pair>
#include <string>
#include <vector>
#include <memory>
#include <map>
// #include "NeuralNet.h"
#include <TFile.h>
#include <TTree.h>
#include <boost/variant.hpp>


typedef boost::variant<double, int, float> Numeric;

struct Specification, BranchList;

class Tagger
{
public:
	Tagger(Specification specs);
	~Tagger();
	void test(unsigned from, unsigned to, std::vector<std::vector<double>> &perf_data);
private:
	// NeuralNet net;
	TFile *file;
	TTree *tree;
	bool fail;
	Event event;
	std::map<std::string, Numeric> variables;
	int n_entries, train, test;
};


struct Specification
{
	std::string root_file = "/Users/lukedeoliveira/Documents/root_files/perf_ntuple.root", 
	            tree_name = "SVTree",
	            load_file = "blah";

	std::vector<std::pair<std::string, std::string>> types;

	bool verbose = 0;

	int train = 0,
	    test = 100000;
};





