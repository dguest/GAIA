//------------------------------------------------------
//				Activation.h
//				By: Luke de Oliveira
//------------------------------------------------------
#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#include <cmath>
#include <vector>
#include <utility>

double sig(double x);
double dsig(double x);

std::vector<double> sigmoid(std::vector<double> A);
std::vector<double> dsigmoid(std::vector<double> A);
std::vector<double> softmax(std::vector<double> A);
template <typename T> void vector_print(std::vector<T> v);
std::string trim(const std::string& str, const std::string& whitespace = " ");

//----------------------------------------------------------------------------

template <typename T>
void vector_print(std::vector<T> v)
{
    for (auto i : v)
    {
        std::cout << i << ",  ";
    }
    std::cout << "\n";
}

#endif
