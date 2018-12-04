#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>

int main(int argc, char** argv) {
	std::string input_filepath;
	ArgParse::ArgParser Parser("Test");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	std::ifstream infile(input_filepath.c_str());

	int frequency = 0;
	int f_diff = 0;
	while(infile >> f_diff) {
		frequency += f_diff;
	}

	std::cout << "Final Frequency: " << frequency << std::endl;

	return 0;
}
