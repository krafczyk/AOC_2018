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

void print_scores(const std::vector<int>& scores, size_t elf_idx_1, size_t elf_idx_2) {
	for(size_t idx = 0; idx < scores.size(); ++idx) {
		if(idx == elf_idx_1) {
			std::cout << "(" << scores[idx] << ")";
		} else if(idx == elf_idx_2) {
			std::cout << "[" << scores[idx] << "]";
		} else {
			std::cout << " " << scores[idx] << " ";
		}
	}
	std::cout << " " << std::endl;
}

int main(int argc, char** argv) {
	// Parse Arguments
	bool verbose = false;
	size_t num_recipies = 0;
	std::string expected;
	bool expected_passed = false;
	ArgParse::ArgParser Parser("Task 27");
	Parser.AddArgument("-n/--num-recipies", "Number of recipies the elves should make before finding the next 10", &num_recipies);
	Parser.AddArgument("-t/--test-value", "Expected result", &expected, ArgParse::Argument::Optional, &expected_passed);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	std::vector<int> recipies;
	recipies.push_back(3);
	recipies.push_back(7);
	size_t elf_idx_1 = 0;
	size_t elf_idx_2 = 1;
	while(recipies.size() < num_recipies+10) {
		if(verbose) {
			print_scores(recipies, elf_idx_1, elf_idx_2);
		}
		// Create new recipies
		size_t sum = recipies[elf_idx_1]+recipies[elf_idx_2];
		if((sum / 10) != 0) {
			recipies.push_back(sum/10);
		}
		recipies.push_back(sum%10);
		// Advance Elfs
		elf_idx_1 = (elf_idx_1+recipies[elf_idx_1]+1)%recipies.size();
		elf_idx_2 = (elf_idx_2+recipies[elf_idx_2]+1)%recipies.size();
	}

	std::stringstream ss;

	for(size_t idx = 0; idx < 10; ++idx) {
		ss << recipies[num_recipies+idx];
	}

	std::cout << "The next ten scores are (" << ss.str() << ")" << std::endl;
	if(expected_passed) {
		if(ss.str() == expected) {
			std::cout << "Test Passed!" << std::endl;
			return 0;
		} else {
			std::cout << "Test Failed!" << std::endl;
			return -1;
		}
	}

	return 0;
}
