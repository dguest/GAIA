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
// struct Reweighting;
struct Reweighting
{
	std::vector<std::vector<double> > charm_correction{{ 4.7813786 ,  5.75476992,  5.32571972},
                                                       { 2.76522175,  3.07056562,  3.49735229},
                                                       { 1.99229081,  2.1827376 ,  2.49293546},
                                                       { 1.69717955,  1.84160008,  2.14931622},
                                                       { 1.6792711 ,  1.7875187 ,  2.2964892 },
                                                       { 1.75172414,  1.95298897,  3.15380736},
                                                       { 2.15092817,  2.67375887,  7.44715447}};

    std::vector<std::vector<double> > bottom_correction{{ 4.21503718,  4.58221626,  4.2480832 },
                                                        { 2.02927332,  2.17348208,  2.3599827 },
                                                        { 1.13992942,  1.22582201,  1.40836526},
                                                        { 0.76866489,  0.82876502,  1.04253533},
                                                        { 0.67610282,  0.73204141,  0.98333168},
                                                        { 0.72593552,  0.84125   ,  1.47890655},
                                                        { 1.06463726,  1.55198777,  4.25255339}};
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











// struct Specification
// {
// 	std::string root_file = "/Users/lukedeoliveira/Documents/root_files/perf_ntuple.root", 
// 	            tree_name = "SVTree";
// 	std::vector<std::pair<std::string, std::string>> input_types;
// 	std::vector<std::pair<std::string, std::string>> output_types;
// 	bool verbose = 0;
// 	int n_entries, train, test;
// };

struct Numeric
{
	double double_;

	float  float_;

	int    int_;

	bool   isDbl = 0, 
	       isInt = 0,
	       isFlt = 0; 
};

inline double cast_as_double(Numeric number);
inline int cast_as_int(Numeric number);








