#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <JetTagger.h>

// utility function to test out the stringstream configuration method
void add_to_stream(const char* file_name, std::stringstream& spec_stream)
{ 
    std::ifstream spec_file( file_name );
    std::string line;
    while (std::getline(spec_file, line) ) 
    {
        spec_stream << line << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    JetTagger::NeuralNet net;

    const char* spec_name = argc > 1 ? argv[1] : "newspecs"; 
    const char* net_name =  argc > 2 ? argv[2] : "example.nnet"; 

    // There are two ways to load the NN "specs" and "net" files: 
    // either pass the file name directly as a string, or (for Athena)
    // pass the text directly as a stringstream. 
    std::stringstream spec_stream; 
    add_to_stream(spec_name, spec_stream); 
    bool spec_good = net.load_specifications(spec_stream); 
    if (!spec_good) 
    { 
        printf("spec not good\n"); 
        return -1; 
    }

    std::stringstream net_stream; 
    add_to_stream(net_name, net_stream); 
    bool net_good = net.load_net(net_stream);
    if (!net_good) 
    { 
        printf("net not good\n"); 
        return -1; 
    }


    //just make phony data to see if it works.
    std::map<std::string, double> data ; 

    // the NN will throw a range_error if the pt and eta are outside the 
    // range on which it's been trained (pt > 15 GeV, |eta| < 2.5). 
    data["pt"] =  30.0; 
    data["eta"] =  1.0; 
    data["nSingleTracks"] =  1.0; 
    data["nTracks"] =  1.0; 
    data["nTracksAtVtx"] =  1.0; 
    data["nVTX"] =  1.0; 
    data["SV1"] =  1.0; 
    data["SV0"] =  1.0; 
    data["ip3d_pb"] =  1.0; 
    data["ip3d_pu"] =  1.0; 
    data["ip3d_pc"] =  1.0; 
    data["jfit_efrc"] =  1.0; 
    data["jfit_mass"] =  1.0; 
    data["jfit_sig3d"] =  1.0; 
    data["svp_mass"] =  1.0; 
    data["svp_efrc"] =  1.0; 
    data["energyFraction"] =  1.0; 
    data["mass"] =  1.0; 
    data["maxSecondaryVertexRho"] =  1.0; 
    data["maxTrackRapidity"] =  1.0; 
    data["meanTrackRapidity"] =  1.0; 
    data["minTrackRapidity"] =  1.0; 
    data["significance3d"] =  1.0; 
    data["subMaxSecondaryVertexRho"] =  1.0; 
    data["jfit_nvtx"] =  1.0; 
    data["jfit_nvtx1t"] =  1.0; 
    data["jfit_ntrkAtVx"] =  1.0;


    std::map<std::string,double> post = net.predict(data);

    for (std::map<std::string,double>::const_iterator entry = post.begin(); 
       entry != post.end(); entry++)
    {
        std::cout << entry->first << ": " << entry->second << std::endl;
    }
    return 0;
}
