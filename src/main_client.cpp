//------------------------------------------------------
//                main_client.cpp
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

int main(int argc, char *argv[]) 
{

//-----------------------------------------------------------------------------
// Declaratiion of command line parsing variables
//-----------------------------------------------------------------------------
    std::string net_file, 
                write_filename, 
                tree_name, 
                root_filename;

    int n_train = 0, 
        n_test = 0;
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
                ++i;
            } 
            else if ((std::string(argv[i]) == "-tree"))  
            {
                tree_name = std::string(argv[i + 1]);
                ++i;
            }
            else if ((std::string(argv[i]) == "-v")) 
            {
                verbose = 1;
            } 
            else if ((std::string(argv[i]) == "-load") && !(load_flag))  
            {
                net_file = std::string(argv[i + 1]);
                ++i;
            }
            else if ((std::string(argv[i]) == "-w") && !(write_flag))  
            {
                write_filename = std::string(argv[i + 1]);
                ++i;
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

    if (net_file.empty())) 
    {
        std::cout << "Error: Executable must be passed a neural net file" << std::endl;
        return -1;
    }

    if (write_filename.empty()) 
    {
        std::cout << "Error: Executable must be passed a filename to write ." << std::endl;
        return -1;
    }
    if ((resume) & (!load_flag)) 
    {
        std::cout << "Error: To resume training, you must pass a .nnet file with the -load flag." << std::endl;
        return -1;
    }
    if (write_flag && (!load_flag)) 
    {
        std::cout << "Error: To write out discriminator distributions, you must load a .nnet file." << std::endl;
    }
//-----------------------------------------------------------------------------
//  Create a default network for analysis
//-----------------------------------------------------------------------------
    structure.push_back(18);
    structure.push_back(20);   
    structure.push_back(7);
    NeuralNet net(structure, classification, sse_flag);
//-----------------------------------------------------------------------------
//  Train from ROOT file
//-----------------------------------------------------------------------------

    //make a place to dump the event into.
    Event event; 

    // load in the root file, tree
    // "~/Documents/root_files/perf_ntuple.root"
    TFile file(root_filename.c_str());


    // TTree* tree = (TTree*) (file.Get("perf_tree"));
    // "SVTree"
    TTree* tree = (TTree*) (file.Get(tree_name.c_str()));
    if (!tree)  
    { 
        std::cout << "Error: Specified TTree " << tree_name << " non-existent." << std::endl;
        return -1;
    }
//-----------------------------------------------------------------------------
//  Loading from previous session
//-----------------------------------------------------------------------------
    std::cout << "\nLoading NeuralNet file:";
    net.load(net_file);
    std::cout << std::setw(7) << "Done." << std::endl;
    //----------------------------------------------------------------------------
    tree->SetBranchAddress("charm", &event.charm);
    tree->SetBranchAddress("bottom", &event.bottom);
    tree->SetBranchAddress("light", &event.light);
    tree->SetBranchAddress("cat_pT", &event.cat_pT);
    tree->SetBranchAddress("cat_eta", &event.cat_eta);
    tree->SetBranchAddress("energyFraction", &event.energyFraction);
    tree->SetBranchAddress("mass", &event.mass);
    tree->SetBranchAddress("significance3d", &event.significance3d);
    tree->SetBranchAddress("nVTX", &event.nVTX);
    tree->SetBranchAddress("nTracksAtVtx", &event.nTracksAtVtx);
    tree->SetBranchAddress("nSingleTracks", &event.nSingleTracks);
    tree->SetBranchAddress("meanTrackRapidity", &event.meanTrackRapidity);
    tree->SetBranchAddress("minTrackRapidity", &event.minTrackRapidity);
    tree->SetBranchAddress("maxTrackRapidity", &event.maxTrackRapidity);
    tree->SetBranchAddress("maxSecondaryVertexRho", &event.maxSecondaryVertexRho);
    tree->SetBranchAddress("subMaxSecondaryVertexRho", &event.subMaxSecondaryVertexRho);
    tree->SetBranchAddress("SVInfoPlus_mass", &event.SVInfoPlus_mass);
    tree->SetBranchAddress("SVInfoPlus_energyfrac", &event.SVInfoPlus_energyfrac);
    tree->SetBranchAddress("SVInfoPlus_normdist", &event.SVInfoPlus_normdist);
    tree->SetBranchAddress("n_SVInfoPlus_gt_svx", &event.n_SVInfoPlus_gt_svx);
    tree->SetBranchAddress("n_SVInfoPlus_2t", &event.n_SVInfoPlus_2t);
    tree->SetBranchAddress("IP3D", &event.IP3D);
    tree->SetBranchAddress("JetPt", &event.JetPt);
    tree->SetBranchAddress("JetEta", &event.JetEta);
    //----------------------------------------------------------------------------
    if (!n_test) 
    {
        n_test = tree->GetEntries();
    }

    double pct;
    //----------------------------------------------------------------------------
    if (eff_flag) 
    {
        std::cout << "Calculating c-jet efficiencies for CharmTagger:\nGetting Predictions:\n";
        std::vector<std::vector<double> > eff_array;
        double b_discriminator;
        combNN combNN_outs;
        tree->SetBranchAddress("logCbJetFitterCOMBNN", &combNN_outs.logCbJetFitterCOMBNN);
        tree->SetBranchAddress("logCuJetFitterCOMBNN", &combNN_outs.logCuJetFitterCOMBNN);
        tree->SetBranchAddress("logBuJetFitterCOMBNN", &combNN_outs.logBuJetFitterCOMBNN);
        tree->SetBranchAddress("discriminatorMV1", &b_discriminator);
        std::vector<double> ENTRY(19);
        // cout << "HERE" << endl;
        if (write_flag) 
        {
            ofstream write_file (write_filename);
            if (write_file.is_open()) 
            {
                write_file << "weight, prob_u, prob_b, prob_c";
                write_file << ", light, bottom, charm";
                write_file << ", Jet_pT, Jet_eta, MV1";
                write_file << ", logCB_jfcombnn, logCU_jfcombnn, logBU_jfcombnn\n";
                for (int entry = n_train; entry < (n_train + n_test); ++entry) 
                {
                    tree->GetEntry(entry);
                    
                    if (event.JetPt >= 20)
                    {
                        std::vector<double> tags = get_tags(event);
                        get_predictors(event, ENTRY);
                        std::vector<double> val = net.predict(ENTRY);
                        std::vector<double> eff_entry;
                        eff_entry.push_back(log(val[2] / val[0]));
                        eff_entry.push_back(log(val[2] / val[1]));
                        eff_entry.push_back(tags[0]);
                        eff_entry.push_back(tags[1]);
                        eff_entry.push_back(tags[2]);
                        eff_array.push_back(eff_entry);
                        write_file << re_weight(event) << ", ";
                        for(auto entry : val) 
                        {
                            write_file << std::setprecision(8) << entry << ", ";
                        }
                        write_file << tags[0] << ", " << tags[1] << ", " << tags[2];
                        write_file << ", " << event.JetPt << ", " << event.JetEta << ", " << b_discriminator;
                        write_file << ", " << combNN_outs.logCbJetFitterCOMBNN <<  ", " << combNN_outs.logCuJetFitterCOMBNN << ", " << combNN_outs.logBuJetFitterCOMBNN << "\n";
                    }

                    pct = (((double)(entry - n_train)) / ((double) (n_test))) * 100;
                    progress_bar(pct);
                    // std::cout << std::setprecision(3) << "\rGetting predictions: " << std::setw(3) << pct << std::setw(3) << "% complete.";           
                }
            }
            else 
            {
                std::cout << "Error: Unable to create file to catch output stream. Terminating with with error." << std::endl;
                return -1;
            }
            write_file.close();
        }
    }
        //----------------------------------------------------------------------------

    if (verbose)
    {
        std::cout << "\nDone." << std::endl;
    }
    // cin.ignore();
    delete tree;
	return 0;
}
//-----------------------------------------------------------------------------
//  End of implementation
//-----------------------------------------------------------------------------