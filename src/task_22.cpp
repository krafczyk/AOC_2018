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

int cell_power_level(int x, int y, int grid_serial) {
	int rackid = x + 10;
	int power_level = rackid*y;
	power_level += grid_serial;
	power_level = power_level*rackid;
	power_level = power_level/100;
	power_level = power_level%10;
	power_level -= 5;
	return power_level;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	int serial_number = 0;
	int grid_size = 300;
	int test_answer = 0;
	bool test_answer_passed = false;
	ArgParse::ArgParser Parser("Task 22");
	Parser.AddArgument("-s/--serial-number", "Pass in serial number", &serial_number);
	Parser.AddArgument("-gs/--grid-size", "Pass in grid size", &grid_size, ArgParse::Argument::Optional);
	Parser.AddArgument("-t", "Indicate that this is a test and this is the result", &test_answer, ArgParse::Argument::Optional, &test_answer_passed);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Initialize grid
	int grid_values[grid_size][grid_size];
	for(int i_idx=0; i_idx< grid_size; ++i_idx) {
		for(int j_idx=0; j_idx < grid_size; ++j_idx) {
			grid_values[i_idx][j_idx] = cell_power_level(i_idx+1,j_idx+1, serial_number);
		}
	}

	// Optimize over grid size
	int largest_power_val = std::numeric_limits<int>::min();
	int x_min = 0;
	int y_min = 0;
	int size_min = 0;
	for(int mini_grid_size=1; mini_grid_size <=grid_size; ++mini_grid_size) {
		// Find highest power cell
		int largest_power_val_sub = std::numeric_limits<int>::min();
		int x_min_sub = 0;
		int y_min_sub = 0;
		for(int i_idx=0; i_idx< grid_size-mini_grid_size; ++i_idx) {
			for(int j_idx=0; j_idx < grid_size-mini_grid_size; ++j_idx) {
				int mini_grid_sum = 0;
				for(int i=0; i<mini_grid_size; ++i) {
					for(int j=0; j< mini_grid_size; ++j) {
						mini_grid_sum += grid_values[i_idx+i][j_idx+j];
					}
				}
				if(mini_grid_sum > largest_power_val_sub) {
					x_min_sub = i_idx+1;
					y_min_sub = j_idx+1;
					largest_power_val_sub = mini_grid_sum;
				}
			}
		}
		if(largest_power_val_sub > largest_power_val) {
			largest_power_val = largest_power_val_sub;
			x_min = x_min_sub;
			y_min = y_min_sub;
			size_min = mini_grid_size;
		}
	}

	std::cout << "Largest cell group is: " << x_min << "," << y_min << "," << size_min << " with value: " << largest_power_val << std::endl;
	if(test_answer_passed) {
		if(largest_power_val == test_answer) {
			std::cout << "Test Passed!" << std::endl;
			return 0;
		} else {
			std::cout << "Test Failed!" << std::endl;
			return -1;
		}
	}

	return 0;
}
