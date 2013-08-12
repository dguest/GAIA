#include "EfficiencyUtils.h"

bool write_efficiencies(std::vector<std::vector<double> > data, int steps, std::string filename) 
{
    assert(data[0].size() == 5);

    int c_total = 0;
    int l_total = 0;
    int b_total = 0;
    double min_log_CB =0;
    double max_log_CB = 0;
    double min_log_CU =0;
    double max_log_CU = 0;
    double CB_increment, CU_increment, pct;
    std::cout << "\nDetermining cuts:";
    std::cout << "\n";
    for (unsigned int row = 0; row < data.size(); ++row) 
    {
        c_total += data[row][4];
        b_total += data[row][3];
        l_total += data[row][2];

        if (data[row][0] < min_log_CU) 
        {
            min_log_CU = data[row][0];
        }
        if (data[row][0] > max_log_CU) 
        {
            max_log_CU = data[row][0];
        }
        if (data[row][1] < min_log_CB) 
        {
            min_log_CB = data[row][1];
        }
        if (data[row][1] > max_log_CB) 
        {
            max_log_CB = data[row][1];
        }
        pct = (((double)(row)) / ((double) (data.size()))) * 100;
        progress_bar(pct);
        // std::cout << std::setprecision(3) << "\r\nDetermining cuts: " << std::setw(3) << pct << std::setw(3) << "% complete.";    
    }
    std::vector<std::vector<double> > eff_array;
    CB_increment = (max_log_CB - min_log_CB) / steps;
    CU_increment = (max_log_CU - min_log_CU) / steps;
    double charm_cut, bottom_cut, light_cut;
    int count = 0;
    std::ofstream file (filename);
    if (file.is_open()) 
    {
        std::cout << "\nWriting efficiency file:\n";
        for (double CBcut = min_log_CB; CBcut < (max_log_CB + .001); CBcut += CB_increment) 
        {
            for (double CUcut = min_log_CU; CUcut < (max_log_CU + .001); CUcut += CU_increment) 
            {
                charm_cut = 0;
                bottom_cut = 0;
                light_cut = 0;
                std::vector<double> eff_entry;
                for (unsigned int row = 0; row < data.size(); ++row) 
                {
                    if ((data[row][0] > CUcut) && (data[row][1] > CBcut)) 
                    {
                        charm_cut += data[row][4];
                        bottom_cut += data[row][3];
                        light_cut += data[row][2];
                    }
                }
                if ((bottom_cut > 0) && (light_cut > 0)) 
                {
                    file << log10(((double)b_total) / bottom_cut);
                    file << "\t";
                    file << log10(((double)l_total) / light_cut);
                    file << "\t";
                    file << charm_cut / ((double)c_total);
                    file << "\t";
                    file << CBcut;
                    file << "\t";
                    file << CUcut;
                    file << "\n";
                }
            }
            
            pct = (((double)(count)) / ((double) (steps))) * 100;
            count++;
            progress_bar(pct);    
        }
    }
    else 
    {
        std::cout << "Error: Unable to create file to catch output stream. Terminating with with error." << std::endl;
        return -1;
    }
    file.close();
    return 1;
}
//----------------------------------------------------------------------------
bool b_write_efficiencies(std::vector<std::vector<double> > data, int steps, std::string filename) 
{
    int c_total = 0;
    int l_total = 0;
    int b_total = 0;
    double min_log_BU = 99;
    double max_log_BU = -99;
    double BU_increment, pct;
    std::cout << "\nDetermining cuts: ";
    std::cout << "\n";
    std::vector<std::vector<double> > eff_array;

    for (unsigned int row = 0; row < data.size(); ++row) 
    {
        c_total += data[row][3];
        b_total += data[row][2];
        l_total += data[row][1];
        if (data[row][0] < min_log_BU) 
        {
            min_log_BU = data[row][0];
        }
        if (data[row][0] > max_log_BU) 
        {
            max_log_BU = data[row][0];
        }
        pct = (((double)(row)) / ((double) (data.size()))) * 100;
        progress_bar(pct);    
    }

    

    BU_increment = (max_log_BU - min_log_BU) / steps;
    double charm_cut, bottom_cut, light_cut;
    int count = 0;
    std::ofstream file (filename);

    if (file.is_open()) 
    {
        std::cout << "\nWriting efficiency file:\n";
        for (double BUcut_lower = min_log_BU; BUcut_lower < (max_log_BU + .001); BUcut_lower += BU_increment) 
        {
            charm_cut = 0;
            bottom_cut = 0;
            light_cut = 0;
            std::vector<double> eff_entry;
            for (unsigned int row = 0; row < data.size(); ++row) 
            {
                if ((data[row][0] >= BUcut_lower)) 
                {
                    charm_cut += data[row][3];
                    bottom_cut += data[row][2];
                    light_cut += data[row][1];
                }
            }
            if ((charm_cut > 0) && (light_cut > 0)) 
            {
                file << (((double)c_total) / charm_cut);
                file << "\t";
                file << (((double)l_total) / light_cut);
                file << "\t";
                file << bottom_cut / ((double)b_total);
                file << "\t";
                file << BUcut_lower;
                file << "\n";
            }
            pct = (((double)(count)) / ((double) (steps))) * 100;
            count++;
            progress_bar(pct);
        }
        
    

    }
    else 
    {
        std::cout << "Error: Unable to create file to catch output stream. Terminating with with error." << std::endl;
        return -1;
    }
    file.close();
    return 1;
}
//----------------------------------------------------------------------------
bool window_write_efficiencies(std::vector<std::vector<double> > data, int steps, std::string filename) 
{
    assert(data[0].size() == 4);

    int c_total = 0;
    int l_total = 0;
    int b_total = 0;
    double min_log_BU =0;
    double max_log_BU = 0;
    double BU_increment, pct;
    std::cout << "\nDetermining cuts: ";
    std::cout << "\n";
    for (unsigned int row = 0; row < data.size(); ++row) 
    {
        c_total += data[row][3];
        b_total += data[row][2];
        l_total += data[row][1];
        if (data[row][0] < min_log_BU) 
        {
            min_log_BU = data[row][0];
        }
        if (data[row][0] > max_log_BU) 
        {
            max_log_BU = data[row][0];
        }
        pct = (((double)(row)) / ((double) (data.size()))) * 100;
        progress_bar(pct); 
    }

    std::vector<std::vector<double> > eff_array;

    BU_increment = (max_log_BU - min_log_BU) / steps;

    double charm_cut, bottom_cut, light_cut;

    

    int count = 0;

    std::ofstream file (filename);
    if (file.is_open()) 
    {
        std::cout << "\nWriting efficiency file:\n";

        for (double BUcut_upper = min_log_BU; BUcut_upper < (max_log_BU + .001); BUcut_upper += BU_increment) 
        {
            for (double BUcut_lower = min_log_BU; BUcut_lower < BUcut_upper; BUcut_lower += BU_increment) 
            {
                charm_cut = 0;
                bottom_cut = 0;
                light_cut = 0;
                std::vector<double> eff_entry;
                for (unsigned int row = 0; row < data.size(); ++row) 
                {
                    if ((data[row][0] >= BUcut_lower) && (data[row][0] <= BUcut_upper)) 
                    {
                        charm_cut += data[row][3];
                        bottom_cut += data[row][2];
                        light_cut += data[row][1];
                    }
                }
                if ((charm_cut > 0) && (light_cut > 0)) 
                {
                    file << log10(((double)b_total) / bottom_cut);
                    file << "\t";
                    file << log10(((double)l_total) / light_cut);
                    file << "\t";
                    file << charm_cut / ((double)c_total);
                    file << "\n";
                }
            }
            pct = (((double)(count)) / ((double) (steps))) * 100;
            count++;
            progress_bar(pct);
        }
        
       

    }
    else 
    {
        std::cout << "Error: Unable to create file to catch output stream. Terminating with with error." << std::endl;
        return -1;
    }
    file.close();
    return 1;
}