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

/*
template<typename T>
class sparse_state {
	public:
		sparse_state() {
		};
		bool operator[](T idx) {
			if(hasElement(state, idx)) {
				return true;
			} else {
				return false;
			}
		}
		void set(T idx) {
			this->state.insert(idx);
		}
		void remove(T idx) {
			this->state.remove(idx);
		}
		T lowest() const {
			return *state.begin();
		}
		T highest() const {
			return *(state.end()-1);
		}
	private:
		std::set<T> state;
};
*/

void print_state(bool* state, long size) {
	for(long idx=0; idx < size; ++idx) {
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
			for(long i=0; i<5; ++i) {
				this->the_rule[i] = the_rule[i];
			}
		}
		rule(std::string& string_rule) {
			for(long i=0; i<5; ++i) {
				if(string_rule[i] == '#') {
					the_rule[i] = true;
				} else {
					the_rule[i] = false;
				}
			}
		}
		bool match(bool* state, long center) const {
			bool matches = true;
			for(long i=0; i<5; ++i) {
				if(state[center+i-2] != the_rule[i]) {
					matches = false;
					break;
				}
			}
			return matches;
		}
		template<typename T>
		bool match(typename std::set<T>::iterator it) {
			// Iterator should start
		}
		void print() const {
			for(long i=0; i<5; ++i) {
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
	ArgParse::ArgParser Parser("Task 24");
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

	// initialize state
	std::set<long> state;
	std::set<long> state_next;

	for(unsigned long idx = 0; idx < init_state.size(); ++idx) {
		if (init_state[idx] == '#') {
			state.insert(idx);
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
		for(unsigned long idx=0; idx < rules.size(); ++idx) {
			rules[idx].print();
		}
	}

	if(verbose) {
		std::cout << std::setw(3) << std::setfill(' ') << 0 << ": ";
		print_state(state, num_places);
	}
	long num_gen = 50000000000;
	long step = 0;
	while(step < num_gen) {
		for(long i=2; i<num_places-2; ++i) {
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

	long pot_sum = 0;
	for(long i=0; i< num_places; ++i) {
		if(state[i]) {
			pot_sum += idx_to_num(i);
		}
	}
	std::cout << "The sum of all pot numbers is: " << pot_sum << std::endl;

	return 0;
}
