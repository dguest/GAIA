#include <map>
#include <vector>
#include <stdlib.h>
#include <string>
#include <TTree.h>

class Sample
{
public:
	Sample(std::vector<std::string> variable_list);
	Sample(std::vector<std::string> variable_list, TTree *tree);
	~Sample();
	double& operator[](std::string);
	bool link(TTree *tree);

private:
	std::map<std::string, double> entry;
	std::vector<std::string> var_list
};