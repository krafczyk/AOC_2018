#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 9");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);

	while(infile) {
		std::string polymer;
		if(!std::getline(infile, polymer)) {
			break;
		}
		//copy out characters to non-const array
		char new_polymer[polymer.size()];
		for(unsigned int idx=0; idx < polymer.size(); ++idx) {
			new_polymer[idx] = polymer[idx];
		}

		unsigned int i_idx = 0;
		unsigned int current_size = polymer.size();
		while((current_size > 1) && (i_idx < current_size-1)) {
			if(abs(new_polymer[i_idx]-new_polymer[i_idx+1]) == 32) {
				// we now need to copy down by two
				for(unsigned int j_idx = 0; j_idx < current_size-2-i_idx; ++j_idx) {
					new_polymer[i_idx+j_idx] = new_polymer[i_idx+j_idx+2];
				}
				current_size -= 2;
				i_idx = 0;
			} else {
				// Advance otherwise
				i_idx += 1;
			}
		}
		std::cout << "Polymer " << polymer.substr(0,5) << "... reacts down to " << current_size << " units" << std::endl; 
	}

	return 0;
}
