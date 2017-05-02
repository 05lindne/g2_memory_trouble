#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <tclap/CmdLine.h>

#include "custom_exceptions.hpp"

namespace defaults{
   
   const unsigned int g2size = 20000;
   const unsigned int bin_width = 1;
   
   const bool require_paths = false;
   const bool require_constants = false;
   
   // const std::string path_in_1("path_to_input_file_1");
   // const std::string path_in_2("path_to_input_file_2");

   const std::string path_in_1("sample_data/scan_xy_25_25_lr_x26y18_g2_Click_List1.txt");
   const std::string path_in_2("sample_data/scan_xy_25_25_lr_x26y18_g2_Click_List2.txt");
   const std::string path_out("path_to_output_file");
}


template< typename T = int64_t > 
std::vector< T > read_binary_file(const std::string path, const bool verbose = false){

    std::ifstream stream(path, std::ios::binary);
    
    std::vector< T > vec;
    T tmp;
    const unsigned int size = sizeof(tmp);

    // keep pumping numbers from the stream until empty
    while(stream.read(reinterpret_cast<char *>(&tmp), size)){
    
        vec.push_back(tmp);
    } 

    stream.close();

    const bool data_sorted = std::is_sorted(vec.begin(),vec.end());

    if (verbose == true) {

        std::cout << "Loading data from input file: " << path << std::endl;
        std::cout << vec.size() << " records found. Sorted order respected: " << data_sorted << std::endl;
        std::cout << "First is: " << vec.front() << std::endl;
        std::cout << "Last is: " << vec.back() << std::endl;
    }

    if (!data_sorted){
        throw NotSortedException(path);
    }

    return vec;
}

using namespace std;

int main(int argc, char** argv) {

    try {

        TCLAP::CmdLine cmd("Replace this with some text explaining what the program is doing and how to use it.", ' ', "1.0");
        
        TCLAP::ValueArg<string> path_in_1Arg("b","inputfile_2","Path to binary input file 2. The file is expected to hold a sorted list of 64 bit unsinged integers in binary representation.",defaults::require_paths,defaults::path_in_1,"path");
        TCLAP::ValueArg<string> path_in_2Arg("a","inputfile_1","Path to binary input file 1. The file is expected to hold a sorted list of 64 bit unsinged integers in binary representation.",defaults::require_paths,defaults::path_in_2,"path");
        TCLAP::ValueArg<string> path_outArg("o","outputfile","Path to outputfile. Some more description here.",defaults::require_paths,defaults::path_out,"path");
        
        TCLAP::ValueArg<int> g2sizeArg("s","g2size","Number of time bins for binning the g2 function.",defaults::require_constants, defaults::g2size ,"int");
        TCLAP::ValueArg<int> bin_widthArg("w","bin_width","Time width of each bin in units of 4 ps.",defaults::require_constants, defaults::bin_width ,"int");

        TCLAP::SwitchArg verboseSwitch("v","verbose","Print additional information while executing.",cmd, false);

        cmd.parse( argc, argv );

        auto path_in_1 = path_in_1Arg.getValue();
        auto path_in_2 = path_in_2Arg.getValue();
        auto path_out = path_outArg.getValue();
        auto g2size = g2sizeArg.getValue();
        auto bin_width = bin_widthArg.getValue();
        auto verbose = verboseSwitch.getValue();


        // cout << path_in_1 << endl;
        // cout << path_in_2 << endl;
        // cout << path_out << endl;
        // cout << g2size << endl;
        // cout << bin_width << endl;
        // cout << verbose << endl;
       
        // auto data1 = read_binary_file<>(path_in_1, verbose);
        // auto data2 = read_binary_file<>(path_in_2, verbose);

        vector<int64_t> data1 = {1,2,10,11,12,13,15,20};
        vector<int64_t> data2 = {3,4,4,5,8,12};

        cout << "sorted " << is_sorted(data1.begin(),data1.end()) << endl;


        unsigned int insertion_index = 0;
        auto offset = data2.begin();
        int64_t delta = 0;

        for(auto const& e1: data1) {

            

            // insertion_index = bisect_left(data2, item - float(offset)/scale)
            offset = lower_bound(offset,data2.end(),e1); 

            if (offset != data2.end()) {
                // here e1 <= e2 for all e2 in data2. 
                insertion_index = distance(data2.begin(), offset);
                cout << e1 << " to position " << insertion_index << endl;
           

                for(auto it2 = offset; it2 != data2.end(); ++it2) {

                    delta = *it2 - e1;
                    cout << *it2 << " ";

                    // store this delta in a bins array
                }
                cout << endl;

            } else {
                // here e1 > e2 for all e2 in data2. 
                cout << e1 << " to the end" << endl;
            }

            

          

        }


    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    }

    catch(const NotSortedException &e) {
        cerr << "error: " << e.what() << endl;
    }

}



