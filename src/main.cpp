//------------------------------------------------------
//                main.cpp
//              By: Luke de Oliveira
//------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <time.h>
#include <sys/time.h>
#include "NeuralNet.h"
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TSystem.h>

using namespace std;

const std::string _timestamp(void);

int main(int argc, char *argv[]) 
{

//-----------------------------------------------------------------------------
// Declaratiion of command line parsing variables
//-----------------------------------------------------------------------------
    std::string net_file, 
                write_filename, 
                save_filename, 
                string_struct, 
                tree_name, 
                root_filename, 
                spec_file = "";

    bool in_flag = false,
         load_flag = false,
         write_flag = false,
         save_flag = false,
         verbose = false,
         memory = false,
         resume = false,
         struct_flag = false,
         encode = false;

    int n_train = 0, 
        n_test = 0, 
        n_epochs = 20;

    unsigned int holdout = 0;
    std::vector<int> structure;
    double momentum = 0.9, learning = 0.002;
//-----------------------------------------------------------------------------
//  Parse *argv[] for flags
//-----------------------------------------------------------------------------
    for (int i = 1; i < argc; i++)  
    { 
        if (i + 1 <= argc)  
        {
            if ((std::string(argv[i]) == "-root") & !(in_flag))  
            {
                root_filename = std::string(argv[i + 1]);
                in_flag = true;
                ++i;
            } 
            else if ((std::string(argv[i]) == "-spec"))  
            {
                spec_file = std::string(argv[i + 1]);
                ++i;
            } 
            else if ((std::string(argv[i]) == "-tree"))  
            {
                tree_name = std::string(argv[i + 1]);
                ++i;
            } 
            else if ((std::string(argv[i]) == "-v")) 
            {
                verbose = true;
            } 
            else if ((std::string(argv[i]) == "-resume")) 
            {
                resume = true;
            } 
            else if ((std::string(argv[i]) == "-encode")) 
            {
                encode = true;
            } 
            else if ((std::string(argv[i]) == "-memory")) 
            {
                memory = true;
            } 

            else if ((std::string(argv[i]) == "-load") && !(load_flag))  
            {
                net_file = std::string(argv[i + 1]);
                load_flag = true;
                ++i;
            }
            else if ((std::string(argv[i]) == "-w") && !(write_flag))  
            {
                write_filename = std::string(argv[i + 1]);
                write_flag = true;
                ++i;
            }
            else if ((std::string(argv[i]) == "-holdout"))  
            {
                holdout = (int)std::stoi(std::string(argv[i + 1]));
                ++i;
            }
            else if ((std::string(argv[i]) == "-save") && !(save_flag))  
            {
                save_filename = std::string(argv[i + 1]);
                save_flag = true;
                ++i;
            }
            else if ((std::string(argv[i]) == "-struct"))  
            {
                ++i;
                string_struct = std::string(argv[i]);
                std::stringstream iss( string_struct );
                while (getline( iss, string_struct, ',')) 
                {
                    int fieldvalue = 0;
                    std::stringstream( string_struct ) >> fieldvalue;
                    structure.push_back( fieldvalue );
                }
                struct_flag = true;

            }
            else if ((std::string(argv[i]) == "-train"))  
            {
                n_train = (int)std::stoi(std::string(argv[i + 1]));
                ++i;
            } 
            else if ((std::string(argv[i]) == "-test"))  
            {
                n_test = (int)std::stoi(std::string(argv[i + 1]));
                ++i;
            } 
            else if ((std::string(argv[i]) == "-m"))  
            {
                momentum = (double)std::stod(std::string(argv[i + 1]));
                ++i;
            } 
            else if ((std::string(argv[i]) == "-l"))  
            {
                learning = (double)std::stod(std::string(argv[i + 1]));
                ++i;
            } 
            else if ((std::string(argv[i]) == "-epochs"))  
            {
                n_epochs = (int)std::stoi(std::string(argv[i + 1]));
                ++i;
            } 
            else  
            {
                std::cout << "Invalid argument \'" << std::string(argv[i]);
                std::cout << "\' passed. Terminating with error.";
                return -1;
            }
        }
    }
//-----------------------------------------------------------------------------
// Catch logical errors
//-----------------------------------------------------------------------------
    bool bad = false;
    if ((!(load_flag)) & (!in_flag)) 
    {
        std::cout << "Error: Executable must be passed either a neural net";
        std::cout << " file or\ntraining tags to make predictions." << std::endl;
        bad = true;
    }

    if ((!struct_flag) & (!load_flag)) 
    {
        std::cout << "Error: Executable must be passed a neural network structure." << std::endl;
        bad = true;
    }
    if ((resume) & (!load_flag)) 
    {
        std::cout << "Error: To resume training, you must pass a .nnet file with the -load flag." << std::endl;
        bad = true;
    }
    if (write_flag && (!load_flag)) 
    {
        std::cout << "Error: To write out discriminator distributions, you must load a .nnet file." << std::endl;
        bad = true;
    }
    if (spec_file == "")
    {
        std::cout << "Error: you must provide a spec file with variables and types present within the TTree." << std::endl;
        bad = true;
    }
    if (bad)
    {
        return -1;
    }
//-----------------------------------------------------------------------------
//  Create a default network for analysis
//-----------------------------------------------------------------------------
    if (load_flag) 
    {
        structure.push_back(18);
        structure.push_back(20);   
        structure.push_back(7);
    }
    NeuralNet net(structure);
//-----------------------------------------------------------------------------
//  Train from ROOT file
//-----------------------------------------------------------------------------

    net.set_dataset(root_filename, tree_name);

    net.load_specifications(spec_file);

    if(!(load_flag)) //training case for charm tag    
    {

        net.setMomentum(momentum);
        net.setLearning(learning);
        if (verbose)
        {
            std::cout << "\nTaggerFramework Training Procedure:\n------------------------------------------------";     
        } 
        if (verbose)
        {
            std::cout << "\nNormalizing input variables:\n";
        }

        net.getTransform(verbose, memory);

        //Train the net!
        //----------------------------------------------------------------------------
        if (verbose)
        {
            std::cout << "Training:\n";
        }

        if (memory && encode)
        {
            net.encode(verbose);
        }
        net.train(n_epochs, n_train, save_filename, verbose, _timestamp());        
    }
//-----------------------------------------------------------------------------
//  Loading from previous session
//-----------------------------------------------------------------------------

    else 
    {
        std::cout << "\nLoading NeuralNet file:";
        net.load(net_file);

        net.write_perf("", n_train, n_test);
        //----------------------------------------------------------------------------
    }

//-----------------------------------------------------------------------------
//  If we save the neural net that we've just trained
//-----------------------------------------------------------------------------
    // if (save_flag) 
    // {
    //     if (verbose)
    //     {
    //         std::cout << "\nSaving parameters to " << save_filename << ":";
    //     }
    //     net.save(save_filename);
    //     if (verbose)
    //     {
    //         std::cout << std::setw(7) << "Done." << std::endl;
    //     }
    // }
    // cin.ignore();
	return 0;
}
//-----------------------------------------------------------------------------
//  End of implementation
//-----------------------------------------------------------------------------


const std::string _timestamp(void)  
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}