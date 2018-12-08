#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>

unsigned int reducePolymer(const std::string& polymer) {
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
	return current_size;
}

char charClass(char in) {
	if (in > 90) {
		// lower case
		return in;
	} else {
		// upper case
		return in+32;
	}
}

std::vector<char> getCharClasses(const std::string& in) {
	std::vector<char> classes;
	for(auto it = in.begin(); it < in.end(); ++it) {
		char the_class = charClass(*it);
		if (std::find(classes.begin(), classes.end(), the_class) == classes.end()) {
			classes.push_back(the_class);
		}
	}
	return classes;
}

std::string removeCharClass(const std::string& in, char the_class) {
	std::string new_polymer = in;
	auto it = new_polymer.begin();
	while(it < new_polymer.end()) {
		if (charClass(*it) == the_class) {
			it = new_polymer.erase(it);
		} else {
			++it;
		}
	}
	return new_polymer;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 10");
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

	std::string polymer;
	if(!std::getline(infile, polymer)) {
		std::cerr << "There was a problem reading the input!!" << std::endl;
		return -1;
	}

	std::vector<char> classes = getCharClasses(polymer);

	std::map<char,int> class_length;

	// Compute reduced polymers
	for(auto class_it = classes.begin(); class_it < classes.end(); ++class_it) {
		class_length[*class_it] = reducePolymer(removeCharClass(polymer, *class_it));
	}

	// Find smallest
	int min_length = std::numeric_limits<int>::max();
	char min_class = -1;
	for(auto class_length_it = class_length.begin(); class_length_it != class_length.end(); ++class_length_it) {
		if (class_length_it->second < min_length) {
			min_length = class_length_it->second;
			min_class = class_length_it->first;
		}
	}

	std::cout << "Polymer " << polymer.substr(0,5) << "... can react down to " << min_length << " units by removing class " << min_class << std::endl; 

	return 0;
}
