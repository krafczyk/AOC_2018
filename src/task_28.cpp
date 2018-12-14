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

std::string get_last_vals(const std::vector<int>& scores, size_t num) {
	std::string answer;
	for(size_t idx = 0; idx<num; ++idx) {
		answer.push_back(scores[scores.size()-1-num+idx]);
	}
	return answer;
}

size_t get_pattern_idx(const std::vector<int>& scores, const std::vector<int>& pattern) {
	if(scores.size() > pattern.size()) {
		for(size_t idx = 0; idx < scores.size()-pattern.size(); ++idx) {
			bool match = true;
			for(size_t idx_2 = 0; idx_2 < pattern.size(); ++idx_2) {
				if(scores[idx+idx_2] != pattern[idx_2]) {
					match = false;
					break;
				}
			}
			if(match) {
				return idx;
			}
		}
	}
	return std::numeric_limits<size_t>::max();
}

int main(int argc, char** argv) {
	// Parse Arguments
	bool verbose = false;
	std::string pattern;
	size_t expected;
	bool expected_passed = false;
	size_t iteration_limit = 0;
	bool iteration_limit_passed = false;
	ArgParse::ArgParser Parser("Task 28");
	Parser.AddArgument("-p/--pattern", "Pattern to look for", &pattern);
	Parser.AddArgument("-t/--test-value", "Expected result", &expected, ArgParse::Argument::Optional, &expected_passed);
	Parser.AddArgument("-l/--iteration-limit", "Limit the iterations", &iteration_limit, ArgParse::Argument::Optional, &iteration_limit_passed);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// change expected into int vector
	std::vector<int> pattern_expected;
	for(size_t idx=0; idx < pattern.size(); ++idx) {
		pattern_expected.push_back(std::atoi(pattern.substr(idx,1).c_str()));
	}

	std::vector<int> recipies;
	recipies.push_back(3);
	recipies.push_back(7);
	size_t elf_idx_1 = 0;
	size_t elf_idx_2 = 1;
	size_t iteration_count = 0;
	while((get_pattern_idx(recipies, pattern_expected)==std::numeric_limits<size_t>::max())&&((!iteration_limit_passed)||(iteration_count < iteration_limit))) {
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
		iteration_count += 1;
	}

	size_t pattern_idx = get_pattern_idx(recipies, pattern_expected);

	std::cout << "The pattern " << pattern << " occured after " << pattern_idx << " entries." << std::endl;
	if(expected_passed) {
		if(pattern_idx == expected) {
			std::cout << "Test Passed!" << std::endl;
			return 0;
		} else {
			std::cout << "Test Failed!" << std::endl;
			return -1;
		}
	}

	return 0;
}
