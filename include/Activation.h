/**
\file Activation.h
\author Luke de Oliveira

*/


#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>
#include <utility>

double sig(double x);
double dsig(double x);

std::vector<double> sigmoid(std::vector<double> A);
std::vector<double> dsigmoid(std::vector<double> A);
std::vector<double> softmax(std::vector<double> A);
void vector_print(std::vector<double> v);

/**
\param str The input string to be stripped of trailing and leading whitespace.
\param whitespace the whitespace string to be removed.
*/
std::string trim(const std::string& str, const std::string& whitespace = " ");
#endif
