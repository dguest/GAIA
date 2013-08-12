#include "VarUtils.h"

void progress_bar(int percent) 
{
    std::string _prog_bar;
    for(int i = 0; i < 50; i++) 
    {
        if (i < (percent/2)) 
        {
            _prog_bar.replace(i,1,"=");
        }
        else if (i == (percent/2)) 
        {
            _prog_bar.replace(i,1,">");
        }
        else 
        {
            _prog_bar.replace(i,1," ");
        }
    }
    std::cout<< "\r" "[" << _prog_bar << "] ";
    std::cout.width( 3 );
    std::cout<< percent << "%     " << std::flush;
}

void epoch_progress_bar(int percent, int epoch, int tot) 
{
    std::string _prog_bar;
    for(int i = 0; i < 50; i++) 
    {
        if (i < (percent/2)) 
        {
            _prog_bar.replace(i,1,"=");
        }
        else if (i == (percent/2)) 
        {
            _prog_bar.replace(i,1,">");
        }
        else 
        {
            _prog_bar.replace(i,1," ");
        }
    }
    std::cout << "\r" "Epoch " << epoch << " of " << tot << ", [" << _prog_bar << "] ";
    std::cout.width( 6 );
    std::cout<< std::setprecision(3) << percent << "%     " << std::flush;
}






void get_predictors(Event input, std::vector<double> &vec) 
{
    vec[0] = input.flavor_weight_SV1;
    vec[1] = input.flavor_weight_SV0;
    vec[2] = input.flavor_weight_SecondSoftMuonTagChi2;
    vec[3] = input.flavor_weight_SoftMuonTagChi2;
    vec[4] = input.flavor_component_ip3d_pu;
    vec[5] = input.flavor_component_ip3d_pb;
    vec[6] = input.flavor_component_ip3d_pc;
    vec[7] = input.flavor_component_jfit_efrc;
    vec[8] = input.flavor_component_jfit_mass;
    vec[9] = input.flavor_component_jfit_sig3d;
    vec[10] = input.flavor_component_jfit_deltaPhi;
    vec[11] = input.flavor_component_jfit_deltaEta;
    vec[12] = input.flavor_component_svp_mass;
    vec[13] = input.flavor_component_svp_efrc;
    vec[14] = input.flavor_component_svp_chi2;
    vec[15] = input.flavor_component_sv0p_mass;
    vec[16] = input.flavor_component_sv0p_efrc;
    vec[17] = input.flavor_component_sv0p_chi2;
    vec[18] = input.flavor_component_jfitvx_theta;
    vec[19] = input.flavor_component_jfitvx_phi;
    vec[20] = input.flavor_component_jfitvx_errtheta;
    vec[21] = input.flavor_component_jfitvx_errphi;
    vec[22] = input.flavor_component_jfitvx_chi2;
    vec[23] = input.deltaeta;
    vec[24] = input.deltaphi;
    vec[25] = input.energyFraction;
    vec[26] = input.mass;
    vec[27] = input.maxSecondaryVertexRho;
    vec[28] = input.maxSecondaryVertexZ;
    vec[29] = input.maxTrackPtRel;
    vec[30] = input.maxTrackRapidity;
    vec[31] = input.meanTrackPtRel;
    vec[32] = input.meanTrackRapidity;
    vec[33] = input.minTrackPtRel;
    vec[34] = input.minTrackRapidity;
    vec[35] = input.significance3d;
    vec[36] = input.subMaxSecondaryVertexRho;
    vec[37] = input.subMaxSecondaryVertexZ;
    vec[38] = input.flavor_component_jfitvx_ndof;
    vec[39] = input.flavor_component_ip3d_ntrk;
    vec[40] = input.flavor_component_jfit_nvtx;
    vec[41] = input.flavor_component_jfit_nvtx1t;
    vec[42] = input.flavor_component_jfit_ntrkAtVx;
    vec[43] = input.flavor_component_ipplus_trk_n;
    vec[44] = input.flavor_component_svp_ntrkv;
    vec[45] = input.flavor_component_svp_n2t;
    vec[46] = input.flavor_component_svp_ndof;
    vec[47] = input.flavor_component_svp_ntrk;
    vec[48] = input.flavor_component_svp_trk_n;
    vec[49] = input.flavor_component_svp_trk_index;
    vec[50] = input.flavor_component_sv0p_isValid;
    vec[51] = input.flavor_component_sv0p_ntrkv;
    vec[52] = input.flavor_component_sv0p_n2t;
    vec[53] = input.flavor_component_sv0p_ndof;
    vec[54] = input.flavor_component_sv0p_ntrk;
    vec[55] = input.flavor_component_sv0p_trk_n;
    vec[56] = input.flavor_component_jftwotrackvertex_n;
    vec[57] = input.flavor_component_jftwotrackvertex_index;
    vec[58] = input.nSingleTracks; 
    vec[59] = input.nTracks; 
    vec[60] = input.nTracksAtVtx; 
    vec[61] = input.nVTX;
    vec[62] = input.cat_pT;
    vec[63] = input.cat_eta;
    // v.reserve(19);

    // vec[0] = input.IP3D;
    // vec[1] = input.cat_pT;
    // vec[2] = input.cat_eta;
    // vec[3] = input.energyFraction;
    // vec[4] = input.mass;
    // vec[5] = input.significance3d;
    // vec[6] = input.nVTX;
    // vec[7] = input.nTracksAtVtx;
    // vec[8] = input.nSingleTracks;
    // vec[9] = input.meanTrackRapidity;
    // vec[10] = input.minTrackRapidity;
    // vec[11] = input.maxTrackRapidity;
    // vec[12] = input.maxSecondaryVertexRho;
    // vec[13] = input.subMaxSecondaryVertexRho;
    // vec[14] = input.SVInfoPlus_mass;
    // vec[15] = input.SVInfoPlus_energyfrac;
    // vec[16] = input.SVInfoPlus_normdist;
    // vec[17] = input.n_SVInfoPlus_gt_svx;
    // vec[18] = input.n_SVInfoPlus_2t;

    // v.push_back ((double) input.JetPt);
    // v.push_back ((double) input.JetEta);
}
//----------------------------------------------------------------------------
std::vector<double> get_tags(Event input, bool bottom_vs_all) 
{
    if (!bottom_vs_all) 
    {
        std::vector<double> v(3);
        v.reserve(3);
        v[0] = ((double) input.light);
        v[1] = ((double) input.bottom);  
        v[2] = ((double) input.charm);
        return std::move(v);
    } 
    else 
    {
        std::vector<double> v;
        v.push_back((double) input.bottom);
        if (input.bottom == 0) 
        {
            v.push_back(1.000);
        }
        else 
        {
            v.push_back(0.000);
        }
        return std::move(v);
    } 
}

//----------------------------------------------------------------------------
double re_weight(Event event)
{
    if (event.light == 1)
    {
        return 1.0;
    }
    else if (event.bottom == 1)
    {
        return event.bottom_correction[event.cat_pT][event.cat_eta];
    }
    else
    {
        return event.charm_correction[event.cat_pT][event.cat_eta];
    }
}






//----------------------------------------------------------------------------
void usage(void) 
{
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++";
    std::cout << "+++++++++++++++++++++++++++++\n";
    std::cout << " _____ _                        ";
    std::cout << "_____                           \n";
    std::cout << "/  __ \\ |                      ";
    std::cout << "|_   _|                          \n";
    std::cout << "| /  \\/ |__   __ _ _ __ _ __ __";
    std::cout << "_ | | __ _  __ _  __ _  ___ _ __ \n";
    std::cout << "| |   | '_ \\ / _` | '__| '_ ` _";
    std::cout << " \\| |/ _` |/ _` |/ _` |/ _ \\ '__|\n";
    std::cout << "| \\__/\\ | | | (_| | |  | | | |";
    std::cout << " | | | (_| | (_| | (_| |  __/ |   \n";
    std::cout << " \\____/_| |_|\\__,_|_|  |_| |_|";
    std::cout << " |_\\_/\\__,_|\\__, |\\__, |\\___|_|   \n";
    std::cout << "                                ";
    std::cout << "           __/ | __/ |          \n";
    std::cout << "                                ";
    std::cout << "          |___/ |___/           \n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++";
    std::cout << "+++++++++++++++++++++++++++++\n";
    std::cout << "\n";
    std::cout << "    The excecutable CharmTagge";
    std::cout << "r must be called with the following \n";
    std::cout << "    options under the correspo";
    std::cout << "nding scenario:\n";
    std::cout << "\n";
    std::cout << "    NOTE: Random access in a T";
    std::cout << "Tree is of O(n) complexity. Thus, when\n";
    std::cout << "    training a neural network ";
    std::cout << "over many samples, it is advantageous \n";
    std::cout << "    to use sequential access, ";
    std::cout << "as it is O(1). This algorithms proceeds\n";
    std::cout << "    through a TTree IN ORDER, ";
    std::cout << "so be sure to not induce training bias\n";
    std::cout << "    in ordering. When loading ";
    std::cout << "a neural network, the [-train] and [-test]\n";
    std::cout << "    flags tells the net at wha";
    std::cout << "t point training stopped, and how many \n";
    std::cout << "    test points to include. \n";
    std::cout << "\n";
    std::cout << "    Load the pre-trained Charm";
    std::cout << "TaggerV1 Neural Network:\n";
    std::cout << "    ---------------------------------------------------\n";
    std::cout << "\n";    
    std::cout << "    OPTIONS:\n";
    std::cout << "\n";
    std::cout << "        -load:      Load a neural netw";
    std::cout << "ork. To load the pre-optimized\n";
    std::cout << "                    version, use [-load charmnet.nnet]\n";
    std::cout << "\n";
    std::cout << "        -train:     Under loading, it ";
    std::cout << "tells the algorithm where to \n";
    std::cout << "                    start testing. THE";
    std::cout << " DEFAULT IS ZERO, so use caution\n";
    std::cout << "                    to avoid testing o";
    std::cout << "ver training data. Usage is \n";
    std::cout << "                    [-train numberof";
    std::cout << "trainingsamples]. Failure to include\n";
    std::cout << "                    results in zero.\n";
    std::cout << "\n";
    std::cout << "        -test:      Specify the number";
    std::cout << " of test samples to use. In the \n";
    std::cout << "                    loading case, def";
    std::cout << "aults to the number of instances\n";
    std::cout << "                    in the TTree.\n";
    std::cout << "\n";
    std::cout << "        -eff:       Whether or not t";
    std::cout << "o do an analysis of the taggers\n";
    std::cout << "                    efficiency versu";
    std::cout << "s rejection. MUST be followed by\n";
    std::cout << "                    the name of a .tx";
    std::cout << "t file, which will be created or\n";
    std::cout << "                    overwritten, to b";
    std::cout << "e read in by a plotting script.\n";
    std::cout << "\n";
    std::cout << "        -comb:      Writes the effici";
    std::cout << "ency performance of JetFitterCOMBNN\n";
    std::cout << "                    to a file called ";
    std::cout << "'loglikelihoods_CNN.txt'\n";
    std::cout << "\n";
    std::cout << "        -jfc:       Writes the effic";
    std::cout << "iency performance of JetFitterCharm\n";
    std::cout << "                    to a file called";
    std::cout << " 'loglikelihoods_JFC.txt'\n";
}
