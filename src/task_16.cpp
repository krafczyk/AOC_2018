#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <queue>
#include "ArgParseStandalone.h"
#include "utilities.h"

int find_node_value(std::queue<int>& the_queue) {
	int num_child = the_queue.front();
	the_queue.pop();
	int num_meta = the_queue.front();
	the_queue.pop();
	// Calc value
	int answer = 0;
	if (num_child == 0) {
		while(num_meta > 0) {
			answer += the_queue.front();
			the_queue.pop();
			--num_meta;
		}
	} else {
		std::vector<int> child_costs;
		int temp_num_child = num_child;
		while(temp_num_child > 0) {
			child_costs.push_back(find_node_value(the_queue));
			--temp_num_child;
		}
		while(num_meta > 0) {
			int meta_val = the_queue.front();
			the_queue.pop();
			--num_meta;
			if (meta_val == 0) {
				// skip value
				continue;
			} else if (meta_val > num_child) {
				// skip value
				continue;
			} else {
				answer += child_costs[meta_val-1];
			}
		}
	}
	return answer;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 16");
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

	std::vector<int> license_numbers;
	int temp;
	while(infile >> temp) {
		license_numbers.push_back(temp);
	}

	if (verbose) {
		std::cout << "Read the following numbers:" << std::endl;
		for(unsigned int idx = 0; idx < license_numbers.size(); ++idx) {
			std::cout << license_numbers[idx] << " ";
		}
		std::cout << std::endl;
	}

	// Build queue
	std::queue<int> license_nums;
	for(unsigned int idx = 0; idx < license_numbers.size(); ++idx) {
		license_nums.push(license_numbers[idx]);
	}

	std::cout << "The value of the root node is: " << find_node_value(license_nums) << std::endl;

	return 0;
}
