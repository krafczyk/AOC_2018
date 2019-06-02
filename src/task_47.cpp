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

typedef int type;

class group {
    public:
        type units;
        type hp;
        type initiative;
        std::string attack_type;
        std::vector<std::string> weaknesses;
        std::vector<std::string> immunities;
};

class army {
    public:
        std::vector<group> groups;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 47");
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

    // Immune system.
    army immune;

    std::string line;
    std::getline(infile, line);
    while(std::getline(infile, line)) {
        // Quit if we've found the Infection line
        if(line == "Infection:") {
            break;
        }
    }

	return 0;
}
