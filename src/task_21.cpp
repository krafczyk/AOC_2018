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
	int x = 0;
	bool x_passed = false;
	int y = 0;
	bool y_passed = false;
	bool val = false;
	int mini_grid_size = 3;
	int test_answer = 0;
	bool test_answer_passed = false;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-s/--serial-number", "Pass in serial number", &serial_number);
	Parser.AddArgument("-gs/--grid-size", "Pass in grid size", &grid_size, ArgParse::Argument::Optional);
	Parser.AddArgument("-mgs/--mini-grid-size", "Pass in mini grid size", &mini_grid_size, ArgParse::Argument::Optional);
	Parser.AddArgument("-x", "Pass an x value to test", &x, ArgParse::Argument::Optional, &x_passed);
	Parser.AddArgument("-y", "Pass a y value to test", &y, ArgParse::Argument::Optional, &y_passed);
	Parser.AddArgument("--val", "Indicate whether to only print the value of a single cell", &val, ArgParse::Argument::Optional);
	Parser.AddArgument("-t", "Indicate that this is a test and this is the result", &test_answer, ArgParse::Argument::Optional, &test_answer_passed);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	if(val) {
		if((!x_passed)||(!y_passed)) {
			std::cerr << "Please specify x and y!" << std::endl;
			return -1;
		}
		int cell_power = cell_power_level(x,y,serial_number);
		std::cout << "Fuel cell at " << x << "," << y << " grid serial number " << serial_number << " has power level " << cell_power << std::endl;
		if(test_answer_passed) {
			if(cell_power == test_answer) {
				std::cout << "Test Passed!" << std::endl;
				return 0;
			} else {
				std::cout << "Test Failed!" << std::endl;
				return -1;
			}
		}
		return 0;
	}

	if((x_passed)&&(y_passed)) {
		int mini_grid_sum = 0;
		for(int x_val = x; x_val < x+mini_grid_size; ++x_val) {
			for(int y_val=y; y_val < y+mini_grid_size; ++y_val) {
				mini_grid_sum += cell_power_level(x_val, y_val, serial_number);
			}
		}
		std::cout << "Full cell group at " << x << "," << y << " grid serial number " << serial_number << " has power level " << mini_grid_sum << std::endl;
		if(test_answer_passed) {
			if(mini_grid_sum == test_answer) {
				std::cout << "Test Passed!" << std::endl;
				return 0;
			} else {
				std::cout << "Test Failed!" << std::endl;
				return -1;
			}
		}
		return 0;
	}

	// Initialize grid
	int grid_values[grid_size][grid_size];
	for(int i_idx=0; i_idx< grid_size; ++i_idx) {
		for(int j_idx=0; j_idx < grid_size; ++j_idx) {
			grid_values[i_idx][j_idx] = cell_power_level(i_idx+1,j_idx+1, serial_number);
		}
	}

	// Find highest power cell
	int largest_power_val = std::numeric_limits<int>::min();
	int x_min = 0;
	int y_min = 0;
	for(int i_idx=0; i_idx< grid_size-mini_grid_size; ++i_idx) {
		for(int j_idx=0; j_idx < grid_size-mini_grid_size; ++j_idx) {
			int mini_grid_sum = 0;
			for(int i=0; i<mini_grid_size; ++i) {
				for(int j=0; j< mini_grid_size; ++j) {
					mini_grid_sum += grid_values[i_idx+i][j_idx+j];
				}
			}
			if(mini_grid_sum > largest_power_val) {
				x_min = i_idx+1;
				y_min = j_idx+1;
				largest_power_val = mini_grid_sum;
			}
		}
	}

	std::cout << "Largest cell group is: " << x_min << ", " << y_min << " with value: " << largest_power_val << std::endl;
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
