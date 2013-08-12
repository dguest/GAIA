//------------------------------------------------------
//				Activation.h
//				By: Luke de Oliveira
//------------------------------------------------------
#ifndef _ACTIVATION
#define _ACTIVATION

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>
#include <utility>

enum ann_type { regression, classification };
double sig(double x);
double dsig(double x);

std::vector<double> sigmoid(std::vector<double> A);
std::vector<double> dsigmoid(std::vector<double> A);
std::vector<double> softmax(std::vector<double> A);
void vector_print(std::vector<double> v);
std::string trim(const std::string& str, const std::string& whitespace = " ");
#endif
