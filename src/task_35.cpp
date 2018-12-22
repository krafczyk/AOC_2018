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
        lumberyard_state(int in_size) {
            this->size = in_size;
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
                    if((j_diff == 0)&&(i_diff == 0)) {
                        // Skip the middle.
                        continue;
                    }
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
        int size;
        char* lumberyard;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    bool super_verbose = false;
    int size = 50;
    int num_minutes = 0;
	ArgParse::ArgParser Parser("Task 35");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-m/--minutes", "Specify number of minutes", &num_minutes);
    Parser.AddArgument("-s/--size", "Specify size", &size, ArgParse::Argument::Optional);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-sv/--super-verbose", "Print Extra Verbose output", &super_verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    // Create Lumberyards
    lumberyard_state* current_lumberyard = new lumberyard_state(size);
    lumberyard_state* next_lumberyard [[maybe_unused]]= new lumberyard_state(size);

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

    int minutes = 0;
    while(minutes < num_minutes) {
        // Get next lumberyard
        for(int line_idx = 0; line_idx < size; ++line_idx) {
            for(int x_idx = 0; x_idx < size; ++x_idx) {
                auto neighbor_stats = current_lumberyard->neighbor_stats(line_idx, x_idx);
                char tile = (*current_lumberyard)(line_idx,x_idx);
                if(tile == '.') {
                    if(neighbor_stats['|'] >= 3) {
                        next_lumberyard->assign(line_idx, x_idx) = '|';
                    } else {
                        next_lumberyard->assign(line_idx, x_idx) = '.';
                    }
                } else if (tile == '|') {
                    if(neighbor_stats['#'] >= 3) {
                        next_lumberyard->assign(line_idx, x_idx) = '#';
                    } else {
                        next_lumberyard->assign(line_idx, x_idx) = '|';
                    }
                } else if (tile == '#') {
                    if((neighbor_stats['|'] >= 1)&&(neighbor_stats['#']>=1)) {
                        next_lumberyard->assign(line_idx, x_idx) = '#';
                    } else {
                        next_lumberyard->assign(line_idx, x_idx) = '.';
                    }
                } else {
                    throw std::runtime_error("Unknown tile encountered!");
                }
            }
        }
        // Swap pointers
        lumberyard_state* temp = current_lumberyard;
        current_lumberyard = next_lumberyard;
        next_lumberyard = temp;
        // Increment and display info.
        minutes += 1;
        if(super_verbose) {
            std::cout << "Minute " << minutes << std::endl;
            current_lumberyard->print(std::cout);
        }
    }

    if(verbose) {
        std::cout << "Final State:" << std::endl;
        current_lumberyard->print(std::cout);
    }

    // Calculate resource value:
    int num_lumberyards = 0;
    int num_woods = 0;
    for(int line_idx = 0; line_idx < size; ++line_idx) {
        for(int x_idx = 0; x_idx < size; ++x_idx) {
            char tile = (*current_lumberyard)(line_idx,x_idx);
            if(tile == '|') {
                num_woods += 1;
            }
            if(tile == '#') {
                num_lumberyards += 1;
            }
        }
    }

    std::cout << "Resource Value is: " << num_lumberyards*num_woods << std::endl;

	return 0;
}
