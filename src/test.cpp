#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include "Tagger.h"

int main(int argc, char const *argv[])
{
	std::pair <std::string,std::string> var1 ("charm", "int");
	std::pair <std::string,std::string> var2 ("light", "int");
	std::pair <std::string,std::string> var3 ("bottom", "int");

	Specification spec;
	spec.types.push_back(var1);
	spec.types.push_back(var2);
	spec.types.push_back(var3);
	Tagger tagger(spec);
	return 0;
}