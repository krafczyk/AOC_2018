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

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    int test_value = 0;
    bool test_value_given = false;
	ArgParse::ArgParser Parser("Task 39");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-t/--test-val", "Give a test value.", &test_value, ArgParse::Argument::Optional, &test_value_given);
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
    std::string line;

    // get regex
    std::string regex_line;
    std::getline(infile, regex_line);

    if(verbose) {
        std::cout << "Passed Regex: " << regex_line << std::endl;
    }

    std::vector<std::vector<char>> answer_map;
    if(test_value_given) {
        // Read in resulting map
        std::getline(infile, line);

        while(std::getline(infile,line)) {
            std::vector<char> char_line;
            for(size_t idx = 0; idx < line.size(); ++idx) {
                char_line.push_back(line[idx]);
            }
            answer_map.push_back(char_line);
        }
        if(verbose) {
            std::cout << "Expected map: " << std::endl;
            ConstForEach(answer_map, [](const std::vector<char>& char_line) {
                    ConstForEach(char_line, [](char c) {
                            std::cout << c;
                    });
                    std::cout << std::endl;
            });
        }
    }

	return 0;
}
