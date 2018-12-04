#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>
#include <map>

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	ArgParse::ArgParser Parser("Task 4");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	//open box id file
	std::ifstream infile(input_filepath.c_str());

	unsigned int num_two = 0;
	unsigned int num_three = 0;

	std::string box_id;
	while(infile >> box_id) {
		std::map<char, int> letter_map;
		for(unsigned int idx = 0; idx < box_id.size(); ++idx) {
			letter_map[box_id[idx]] += 1;
		}
		bool two = false;
		bool three = false;
		for(auto it=letter_map.begin(); it != letter_map.end(); ++it) {
			if(it->second == 2) {
				two = true;
			} else if (it->second == 3) {
				three = true;
			}
		}
		if (two) {
			num_two += 1;
		}
		if (three) {
			num_three +=1;
		}
	}

	std::cout << "Checksum: " << num_two*num_three << std::endl;

	return 0;
}
