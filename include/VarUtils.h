#ifndef _VARUTILS
#define _VARUTILS 
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdio.h>

struct Event 
{


    double  pt,
            eta,
            flavor_weight_IP3D,
            flavor_weight_MV1,
            flavor_weight_SV1,
            flavor_weight_SV0,
            flavor_weight_SecondSoftMuonTagChi2,
            flavor_weight_SoftMuonTagChi2,
            flavor_component_ip3d_pu,
            flavor_component_ip3d_pb,
            flavor_component_ip3d_pc,
            flavor_component_jfit_efrc,
            flavor_component_jfit_mass,
            flavor_component_jfit_sig3d,
            flavor_component_jfit_deltaPhi,
            flavor_component_jfit_deltaEta,
            flavor_component_svp_mass,
            flavor_component_svp_efrc,
            flavor_component_svp_x,
            flavor_component_svp_y,
            flavor_component_svp_z,
            flavor_component_svp_err_x,
            flavor_component_svp_err_y,
            flavor_component_svp_err_z,
            flavor_component_svp_cov_xy,
            flavor_component_svp_cov_xz,
            flavor_component_svp_cov_yz,
            flavor_component_svp_chi2,
            flavor_component_sv0p_mass,
            flavor_component_sv0p_efrc,
            flavor_component_sv0p_x,
            flavor_component_sv0p_y,
            flavor_component_sv0p_z,
            flavor_component_sv0p_err_x,
            flavor_component_sv0p_err_y,
            flavor_component_sv0p_err_z,
            flavor_component_sv0p_cov_xy,
            flavor_component_sv0p_cov_xz,
            flavor_component_sv0p_cov_yz,
            flavor_component_sv0p_chi2,
            flavor_component_jfitvx_theta,
            flavor_component_jfitvx_phi,
            flavor_component_jfitvx_errtheta,
            flavor_component_jfitvx_errphi,
            flavor_component_jfitvx_chi2,   
            deltaeta, 
            deltaphi, 
            energyFraction, 
            mass, 
            maxSecondaryVertexRho, 
            maxSecondaryVertexZ, 
            maxTrackPtRel, 
            maxTrackRapidity, 
            meanTrackPtRel, 
            meanTrackRapidity, 
            minTrackPtRel, 
            minTrackRapidity, 
            significance3d, 
            subMaxSecondaryVertexRho, 
            subMaxSecondaryVertexZ;


    int flavor_component_jfitvx_ndof,
        flavor_truth_label,
        flavor_component_ip3d_ntrk,
        flavor_component_jfit_nvtx,
        flavor_component_jfit_nvtx1t,
        flavor_component_jfit_ntrkAtVx,
        flavor_component_ipplus_trk_n,
        flavor_component_svp_ntrkv,
        flavor_component_svp_n2t,
        flavor_component_svp_ndof,
        flavor_component_svp_ntrk,
        flavor_component_svp_trk_n,
        flavor_component_svp_trk_index,
        flavor_component_sv0p_isValid,
        flavor_component_sv0p_ntrkv,
        flavor_component_sv0p_n2t,
        flavor_component_sv0p_ndof,
        flavor_component_sv0p_ntrk,
        flavor_component_sv0p_trk_n,
        flavor_component_jftwotrackvertex_n,
        flavor_component_jftwotrackvertex_index,
        nSingleTracks, 
        nTracks, 
        nTracksAtVtx, 
        nVTX,
        bottom,
        charm,
        light,
        cat_pT,
        cat_eta;
    // int charm, bottom, light, nVTX, nTracksAtVtx, nSingleTracks, n_SVInfoPlus_gt_svx, n_SVInfoPlus_2t, cat_pT, cat_eta;
    // double energyFraction, mass, significance3d, meanTrackRapidity, \
    // minTrackRapidity, maxTrackRapidity, maxSecondaryVertexRho, \
    // subMaxSecondaryVertexRho, SVInfoPlus_mass, SVInfoPlus_energyfrac, \
    // SVInfoPlus_normdist, IP3D, JetPt, JetEta;  

    std::vector<std::vector<double> > charm_correction{{ 4.7813786 ,  5.75476992,  5.32571972},
                                                       { 2.76522175,  3.07056562,  3.49735229},
                                                       { 1.99229081,  2.1827376 ,  2.49293546},
                                                       { 1.69717955,  1.84160008,  2.14931622},
                                                       { 1.6792711 ,  1.7875187 ,  2.2964892 },
                                                       { 1.75172414,  1.95298897,  3.15380736},
                                                       { 2.15092817,  2.67375887,  7.44715447}};

    // std::vector<std::vector<double> > charm_correction{{  5.59760313,   6.28293211,   6.78174603,   6.51538352},
    //                                                    {  2.81854947,   3.0717899 ,   3.14447236,   3.71462519},
    //                                                    {  2.23585003,   2.36487482,   2.47982367,   2.99461909},
    //                                                    {  1.90775902,   2.02902299,   2.24015494,   2.82925883},
    //                                                    {  1.77339802,   1.84843982,   1.9284188 ,   2.44797178},
    //                                                    {  1.69045559,   1.80924234,   1.95430334,   2.53740275},
    //                                                    {  1.6976665 ,   1.79863772,   2.02673611,   2.85856079},
    //                                                    {  1.72741029,   1.78027867,   2.18412243,   3.10080183},
    //                                                    {  1.77250247,   1.93143329,   2.29386892,   4.30864198},
    //                                                    {  2.14135626,   2.38079096,   4.08333333,  10.64646465},
    //                                                    {  1.75757576,   5.11111111,   5.5       ,         11.0}};



    std::vector<std::vector<double> > bottom_correction{{ 4.21503718,  4.58221626,  4.2480832 },
                                                        { 2.02927332,  2.17348208,  2.3599827 },
                                                        { 1.13992942,  1.22582201,  1.40836526},
                                                        { 0.76866489,  0.82876502,  1.04253533},
                                                        { 0.67610282,  0.73204141,  0.98333168},
                                                        { 0.72593552,  0.84125   ,  1.47890655},
                                                        { 1.06463726,  1.55198777,  4.25255339}};


    // std::vector<std::vector<double> > bottom_correction{{ 4.87593351,  5.00258442,  5.44049698,  5.24433945},
    //                                                     { 1.91521325,  2.01178594,  2.20515682,  2.39858833},
    //                                                     { 1.22234203,  1.29225012,  1.43055556,  1.68870576},
    //                                                     { 0.9246194 ,  0.98968407,  1.10735257,  1.36904762},
    //                                                     { 0.78119713,  0.81885203,  0.92907144,  1.17024147},
    //                                                     { 0.70233132,  0.75236861,  0.83790086,  1.11216032},
    //                                                     { 0.68535633,  0.72089135,  0.83404778,  1.13385827},
    //                                                     { 0.69561962,  0.74287759,  0.88118392,  1.41166041},
    //                                                     { 0.74878823,  0.86458102,  1.12645349,  2.21446701},
    //                                                     { 1.03337021,  1.3537651 ,  2.3891844 ,  5.29116466},
    //                                                     { 1.72619048,  3.83333333,  6.875     ,         6.0}};

}; 
struct JFC 
{   
   double Likelihood_u;  
   double Likelihood_b; 
   double Likelihood_c;
};
struct combNN 
{
    double logCbJetFitterCOMBNN;
    double logCuJetFitterCOMBNN;
    double logBuJetFitterCOMBNN;
};
void get_predictors(Event input, std::vector<double> &vec); // Get variables from the Event struct
std::vector<double> get_tags(Event input, bool bottom_vs_all = 0); // Get truth tags
void usage(void);
void progress_bar(int percent);
void epoch_progress_bar(int percent, int epoch, int tot);
double re_weight(Event event);

#endif