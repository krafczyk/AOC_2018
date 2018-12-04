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

	std::vector<std::string> box_ids;
	std::string box_id;
	while(infile >> box_id) {
		box_ids.push_back(box_id);
	}

	unsigned int i_idx = 0;
	unsigned int j_idx = 0;
	unsigned int match_k_idx = 0;
	unsigned int id_length = box_ids[0].size();
	int num_mismatches = 0;
	for(i_idx = 0; i_idx < box_ids.size(); ++i_idx) {
		std::string& i_id = box_ids[i_idx];
		for(j_idx = i_idx+1; j_idx < box_ids.size(); ++j_idx) {
			std::string& j_id = box_ids[j_idx];
			// Count mismatches
			num_mismatches = 0;
			for(unsigned int k_idx = 0; k_idx < id_length; ++k_idx) {
				if(i_id[k_idx] != j_id[k_idx]) {
					num_mismatches += 1;
					match_k_idx = k_idx;
				}
				if (num_mismatches >= 2) {
					break;
				}
			}
			if(num_mismatches == 1) {
				break;
			}
		}
		if (num_mismatches == 1) {
			break;
		}
	}
	std::string the_string = box_ids[i_idx];

	the_string.erase(the_string.begin()+match_k_idx);

	std::cout << "Remaining letters: " << the_string << std::endl;

	return 0;
}
