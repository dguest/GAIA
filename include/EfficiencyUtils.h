#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <assert.h>
#include <TMath.h>
#include "VarUtils.h"

bool write_efficiencies(std::vector<std::vector<double> > data, int steps, std::string filename);

//----------------------------------------------------------------------------
bool b_write_efficiencies(std::vector<std::vector<double> > data, int steps, std::string filename);

//----------------------------------------------------------------------------
bool window_write_efficiencies(std::vector<std::vector<double> > data, int steps, std::string filename);


