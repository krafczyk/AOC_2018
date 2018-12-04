#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	ArgParse::ArgParser Parser("Task 2");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	std::vector<int> f_diffs;

	//Read in frequency changes
	std::ifstream infile(input_filepath.c_str());

	int f_diff = 0;
	while(infile >> f_diff) {
		f_diffs.push_back(f_diff);
	}

	int frequency = 0;
	// Keep set of frequencies we've seen
	std::set<int> seen_frequencies;
	seen_frequencies.insert(frequency);

	unsigned int f_current_idx = 0;
	while (true) {
		frequency += f_diffs[f_current_idx];
		f_current_idx += 1;
		if (f_current_idx >= f_diffs.size()) {
			f_current_idx = 0;
		}
		if (seen_frequencies.find(frequency) != seen_frequencies.end()) {
			break;
		}
		seen_frequencies.insert(frequency);
	}

	std::cout << "First repeated frequency: " << frequency << std::endl;

	return 0;
}
