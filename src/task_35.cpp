#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

class lumberyard_state {
    public:
        lumberyard_state() {
            lumberyard = new char[size*size];
            for(int idx = 0; idx < size*size; ++idx) {
                lumberyard[idx] = 0;
            }
        }
        ~lumberyard_state() {
            delete [] lumberyard;
        }
        char& assign(int x_idx, int line_idx) {
            return lumberyard[line_idx*size+x_idx];
        }
        char operator()(int x_idx, int line_idx) const {
            return lumberyard[line_idx*size+x_idx];
        }
        std::map<char,int> neighbor_stats(int i, int j) const {
            std::map<char,int> the_map;
            for(int i_diff = -1; i_diff <= 1; ++i_diff) {
                for(int j_diff = -1; j_diff <= 1; ++j_diff) {
                    int i_idx = i+i_diff;
                    int j_idx = j+j_diff;
                    if((i_idx >= 0)&&(j_idx >= 0)&&
                       (i_idx < size)&&(j_idx < size)) {
                        the_map[(*this)(i_idx,j_idx)] += 1;
                    }
                }
            }
            return the_map;
        }
        void print(std::ostream& out) const {
            for(int line_idx = 0; line_idx < size; ++line_idx) {
                for(int x_idx = 0; x_idx < size; ++x_idx) {
                    out << (*this)(line_idx,x_idx);
                }
                out << std::endl;
            }
        }
    private:
        static const int size = 50;
        char* lumberyard;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 35");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    // Create Lumberyards
    lumberyard_state* current_lumberyard = new lumberyard_state();
    lumberyard_state* next_lumberyard [[maybe_unused]]= new lumberyard_state();

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;
    int line_idx = 0;
    while(std::getline(infile, line)) {
        for(int idx=0; idx < (int) line.size(); ++idx) {
            current_lumberyard->assign(line_idx,idx) = line[idx];
        }
        line_idx += 1;
    }

    if(verbose) {
        std::cout << "Initial State:" << std::endl;
        current_lumberyard->print(std::cout);
    }

	return 0;
}
