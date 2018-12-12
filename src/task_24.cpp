#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

void print_state(bool* state, int size) {
	for(int idx=0; idx < size; ++idx) {
		if(state[idx]) {
			std::cout << '#';
		} else {
			std::cout << '.';
		}
	}
	std::cout << std::endl;
}

class rule {
	public:
		rule(bool* the_rule) {
			for(int i=0; i<5; ++i) {
				this->the_rule[i] = the_rule[i];
			}
		}
		rule(std::string& string_rule) {
			for(int i=0; i<5; ++i) {
				if(string_rule[i] == '#') {
					the_rule[i] = true;
				} else {
					the_rule[i] = false;
				}
			}
		}
		bool match(bool* state, int center) const {
			bool matches = true;
			for(int i=0; i<5; ++i) {
				if(state[center+i-2] != the_rule[i]) {
					matches = false;
					break;
				}
			}
			return matches;
		}
		void print() const {
			for(int i=0; i<5; ++i) {
				if(the_rule[i]) {
					std::cout << '#';
				} else {
					std::cout << '.';
				}
			}
			std::cout << " => #" << std::endl;
		}
		bool the_rule[5];
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 23");
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

	std::string line;
	// Get initial state
	std::getline(infile, line);

	std::string init_state = line.substr(15);

	// Helper variables and function to transition
	int left_extent = -30;
	int right_extent = (int)(line.size()*1.2);
	int num_places = -left_extent+right_extent+1;
	auto idx_to_num [[maybe_unused]] = [left_extent](int idx) {
		return idx+left_extent;
	};
	auto num_to_idx = [left_extent](int num) {
		return num-left_extent;
	};

	// initialize state
	bool* state = new bool[num_places];
	bool* state_next = new bool[num_places];
	for(int i=0; i < num_places; ++i) {
		state[i] = false;
		state_next[i] = false;
	}

	for(unsigned int idx = 0; idx < init_state.size(); ++idx) {
		if (init_state[idx] == '#') {
			state[num_to_idx(idx)] = true;
		}
	}

	// Load rules
	// skip empty line
	std::getline(infile, line);

	std::vector<rule> rules;
	while(std::getline(infile, line)) {
		// A growth rule
		if (line[9] == '#') {
			rules.push_back(rule(line));
		}
	}

	if(verbose) {
		std::cout << "Read in rules are:" << std::endl;
		for(unsigned int idx=0; idx < rules.size(); ++idx) {
			rules[idx].print();
		}
	}

	if(verbose) {
		std::cout << std::setw(3) << std::setfill(' ') << 0 << ": ";
		print_state(state, num_places);
	}
	int num_gen = 20;
	int step = 0;
	while(step < num_gen) {
		for(int i=2; i<num_places-2; ++i) {
			bool rule_match = false;
			for(auto rule_it = rules.begin(); rule_it != rules.end(); ++rule_it) {
				if (rule_it->match(state, i)) {
					rule_match = true;
					break;
				}
			}
			if(rule_match) {
				state_next[i] = true;
			} else {
				state_next[i] = false;
			}
		}
		bool* state_temp = state;
		// Swap states.
		state = state_next;
		state_next = state_temp;
		if(verbose) {
			std::cout << std::setw(3) << std::setfill(' ') << step+1 << ": ";
			print_state(state, num_places);
		}
		++step;
	}

	int pot_sum = 0;
	for(int i=0; i< num_places; ++i) {
		if(state[i]) {
			pot_sum += idx_to_num(i);
		}
	}
	std::cout << "The sum of all pot numbers is: " << pot_sum << std::endl;

	return 0;
}
