#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

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
   const std::string path_out("sample_data/results.txt");
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

    if (verbose == true) {

        std::cout << "Loading data from input file: " << path << std::endl;
        std::cout << vec.size() << " records found."<< std::endl;
        std::cout << "First is: " << vec.front() << std::endl;
        std::cout << "Last is: " << vec.back() << std::endl;
    }

    return vec;
}

template< typename T = int64_t>
std::vector< T > compute_g2_function(const std::vector<T> data1, const std::vector<T> data2, const float size, const float bin_width, const bool verbose = false ) {

    using namespace std;

    if ( !is_sorted(begin(data1),end(data1)) ){
        throw NotSortedException("inputfile_1");
    }
    if ( !is_sorted(begin(data2),end(data2)) ){
        throw NotSortedException("inputfile_2");
    }

    if (size <= 0)
        throw std::runtime_error("Number of bins must be non-negative!");
    if (bin_width <= 0)
        throw std::runtime_error("Bin width must be non-negative!");

    auto offset = begin(data2);         // tracks how much of data2 we have already examined
    auto end_data2 = end(data2);
    unsigned long int bin_index = 0;
   
    const float scale = 1./bin_width;
    float item = 0.0;

    vector<T> bins_array(2*size+1);
    unsigned long int bins_array_size = bins_array.size();

    // we compare elements e1 in data1 with elements e2 in data2
    for(auto const& e1: data1) {

        // defining equation for binning: bin_index = (e2-e1)*scale + size if bin_index >= 0
        // here e1 in data1 and e2 in data2
        // solve for e2 and obtain e2 >= e1 - size/scale = item
        // thus we only need to worry about binning all e2 in data2 for which e2 >= item holds
        // we find this special e2, call it offset, using binary search, hence the input arrays must be sorted
        item = e1 - size/scale;     
        offset = lower_bound(offset,end_data2,item);   
        // note here that subsequent calls of lower_bound may start from offset since due to sorted order we know that we do not need to consider elements that come before offset again   

        if (offset != end_data2) {
            // here e1 <= e2 holds for all e2 in data2 and we may compute the bins for e2 in data2[offset:]
                   
            for(auto it2 = offset; it2 != end_data2; ++it2) {

                bin_index = floor((*it2 - e1) * scale + size);
            
                // cout << bin_index << " ";
                if ( bin_index < bins_array_size) {
                    bins_array[bin_index]++;
                }
            }
            // cout << endl;
         
        } else {
            // here e1 > e2 holds for all e2 in data2. Hence e2 - e1 < 0 and thus no meaningful binning information can be computed. 
            // It follows that we may savely omit the remaining iterations over data1.
            break;
        }   
    }

    return bins_array;
}

template< typename T = std::vector<int64_t> >
void save_results(const T data, const std::string path, const char * seperator = "\t", const bool verbose = true){

    if (verbose) {
        std::cout << "Writing g2 results to file: " << path << "\t\t\t";
    }

    std::ofstream stream(path);
    // int start = addressof(data[0]);
    int index = 0;

    stream << "# this file contains the results of g2 function computation." << "\n";
    stream << "# data format:   bin " << seperator << " count \n";
    stream << "# file 1 was: something" << "\n";
    stream << "# file 2 was: something" << "\n";
    stream << "# setting 1 was: something" << "\n";
    stream << "# setting 2 was: something" << "\n";
    stream << "# bin " << seperator << " count \n";
    

    for(auto const& v: data) {
        index = addressof(v)-addressof(data[0]);                     //compute the index of the array using pointer arithmetic
        stream << index << seperator << v << '\n';      //dump index and value to stream seperated by a seperator  
    }
    stream.close();

    if (verbose) {
        std::cout << "done." << std::endl;  
    }
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

        cmd.add( path_in_1Arg );
        cmd.add( path_in_2Arg );
        cmd.add( path_outArg );
        cmd.add( g2sizeArg );
        cmd.add( bin_widthArg );
        cmd.parse( argc, argv );

        auto path_in_1 = path_in_1Arg.getValue();
        auto path_in_2 = path_in_2Arg.getValue();
        auto path_out = path_outArg.getValue();
        auto g2size = g2sizeArg.getValue();
        auto bin_width = bin_widthArg.getValue();
        auto verbose = verboseSwitch.getValue();


        cout << path_in_1 << endl;
        cout << path_in_2 << endl;
        cout << path_out << endl;
        cout << g2size << endl;
        cout << bin_width << endl;
        cout << verbose << endl;
       
        auto data1 = read_binary_file<>(path_in_1, verbose);
        auto data2 = read_binary_file<>(path_in_2, verbose);

        // const vector<int64_t> data1 = {2,10,11,12,13,15,20};
        // const vector<int64_t> data2 = {3,4,4,5,8,12};

        // auto bins_array = compute_g2_function<>(data1, data2, 15, 1);
        auto bins_array = compute_g2_function<>(data1, data2, g2size, bin_width);


        // for (auto const& v: bins_array) {

        //     cout << v << " ";
        // }
        // cout << endl;

        save_results<>(bins_array, path_out);


    } catch (TCLAP::ArgException &e) {
        std::cerr << "ArgumentError: " << e.error() << " for arg " << e.argId() << std::endl; 
    }

    catch(const NotSortedException &e) {
        cerr << "NotSortedError: " << e.what() << endl;
    }

    catch( const std::runtime_error &e) {
        cerr << "RuntimeError: " << e.what() << endl;
    }

}



