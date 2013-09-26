//------------------------------------------------------
//				Activation.cpp
//				By: Luke de Oliveira
//------------------------------------------------------

#include "Activation.h"



//----------------------------------------------------------------------------
double sig(double x) 
{
	return 1 / (1 + exp(-x));
}

void vector_print(std::vector<double> v)
{
    for (auto i : v)
    {
        std::cout << i << ",  ";
    }
    std::cout << "\n";
}
//----------------------------------------------------------------------------
double dsig(double x) 
{
	return x * (1 - x);
}

//----------------------------------------------------------------------------
std::vector<double> sigmoid(std::vector<double> A) 
{
	for(auto &element : A) 
	{
		element = sig(element);
	}
	return std::move(A);
}
std::vector<double> dsigmoid(std::vector<double> A) 
{
	for(auto &element : A) 
	{
		element = dsig(element);
	}
	return std::move(A);
}

//----------------------------------------------------------------------------
std::vector<double> softmax(std::vector<double> A) 
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

std::string trim(const std::string& str, const std::string& whitespace)
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



