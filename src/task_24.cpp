#include <iostream>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef long pot_num_t;

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

template<typename T>
void print_state(const std::set<T>& state, pot_num_t num_show) {
	typename std::set<T>::const_iterator it = state.begin();
	std::cout << "-" << *it << "- ";
	for(long idx=*it; idx <= *it+num_show; ++idx) {
		if(*it > idx) {
			std::cout << '.';
		} else {
			it = std::find_if(it, state.end(), [idx](T a) { return (a >= idx);});
			if(*it == idx) {
				std::cout << '#';
			} else {
				std::cout << '.';
			}
		}
	}
	std::cout << std::endl;
}

class rule {
	public:
		rule(bool* the_rule) {
			for(pot_num_t i=0; i<5; ++i) {
				this->the_rule[i] = the_rule[i];
			}
		}
		rule(std::string& string_rule) {
			for(pot_num_t i=0; i<5; ++i) {
				if(string_rule[i] == '#') {
					the_rule[i] = true;
				} else {
					the_rule[i] = false;
				}
			}
		}
		bool match(bool* state, pot_num_t center) const {
			bool matches = true;
			for(pot_num_t i=0; i<5; ++i) {
				if(state[center+i-2] != the_rule[i]) {
					matches = false;
					break;
				}
			}
			return matches;
		}
		// state_it should satisfy *state_it <= center-2.
		template<class T>
		bool match(const typename std::set<T>& set, typename std::set<T>::const_iterator state_it, T center) const {
			// Backup iterator to before center-2.
			//typename std::set<T>::reverse_iterator begin = std::find_if(std::reverse_iterator<typename std::set<T>::iterator>(state_it), state.rend(), [center](T a){ return (a <= center-2);});

			bool matches = true;
			for(T i=-2; i <= 2; ++i) {
				// Advance iterator
				state_it = std::find_if(state_it, set.end(), [center,i](T a) { return (a >= center+i); });
				if(the_rule[i+2]) {
					// The iterator should be here
					if(*state_it != center+i) {
						matches = false;
						break;
					}
				} else {
					// The iterator should not be here
					if(*state_it == center+i) {
						matches = false;
						break;
					}
				}
			}
			return matches;
		}
		void print() const {
			for(pot_num_t i=0; i<5; ++i) {
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

pot_num_t CalcPotSum(const std::set<pot_num_t>& state) {
	pot_num_t pot_sum = 0;
	for(auto it = state.cbegin(); it != state.cend(); ++it) {
		pot_sum += *it;
	}
	return pot_sum;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	int num_show = 40;
	pot_num_t num_gen = 20;
	bool no_early_quit = false;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 24");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-n/--num-gen", "Number of generations to do", &num_gen);
	Parser.AddArgument("-ns/--num-show", "Specify number of characters to show", &num_show, ArgParse::Argument::Optional);
	Parser.AddArgument("-neq", "Specify that the algorithm shouldn't just stop when the answer becomes recurrent", &no_early_quit, ArgParse::Argument::Optional);
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
	std::set<pot_num_t>* state = new std::set<pot_num_t>();
	std::set<pot_num_t>* state_next = new std::set<pot_num_t>();
	std::set<pot_num_t> tried;

	for(pot_num_t idx = 0; idx < (pot_num_t) init_state.size(); ++idx) {
		if (init_state[idx] == '#') {
			state->insert(idx);
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
		for(pot_num_t idx=0; idx < (pot_num_t) rules.size(); ++idx) {
			rules[idx].print();
		}
	}

	if(verbose) {
		std::cout << std::setw(3) << std::setfill(' ') << 0 << ": ";
		print_state(*state, num_show);
	}

	pot_num_t step = 0;
	pot_num_t prev_step_sum = CalcPotSum(*state);
	bool recurrent = false;
	// We can't do this brute force, we go until the solution becomes recurrent..
	while((step < num_gen)&&(no_early_quit||(!recurrent))) {
		// clear the tried set
		tried.clear();
		// clear the next set
		state_next->clear();

		typename std::set<pot_num_t>::const_iterator main_it = state->begin();
		// Outer loop iterates through the non-zero state elements themselves.
		while(main_it != state->end()) {
			pot_num_t center = *main_it;
			// Inner loop iterates through possible new pots around
			// the current main_it.
			std::set<pot_num_t>::const_reverse_iterator temp_it(main_it);
			// Reverse the iterator at most two steps
			if(*temp_it > center-2) {
				++temp_it;
				if(*temp_it > center-2) {
					++temp_it;
				}
			}
			std::set<pot_num_t>::const_iterator sub_it = main_it;
			for(pot_num_t i = -2; i <= 2; ++i) {
				if (!hasElement(tried, center+i)) {
					bool matched = false;
					for(auto rule_it = rules.begin(); rule_it != rules.end(); ++rule_it) {
						std::set<pot_num_t>::const_iterator sub_sub_it = sub_it;
						if(rule_it->match(*state, sub_sub_it, center+i)) {
							matched = true;
							break;
						}
					}
					// Mark down center+i as having been tried
					tried.insert(center+i);
					// add it to the next state if a rule matched.
					if(matched) {
						state_next->insert(center+i);
					}
				}
			}
			// Advance main iterator
			++main_it;
		}

		// Swap states.
		std::set<pot_num_t>* state_temp = state;
		state = state_next;
		state_next = state_temp;

		if (CalcPotSum(*state)-prev_step_sum == (pot_num_t) state->size()) {
			recurrent = true;
		}
		prev_step_sum = CalcPotSum(*state);
		if(verbose) {
			std::cout << std::setw(3) << std::setfill(' ') << step+1 << ": ";
			print_state(*state, num_show);
		}
		++step;
	}

	// Solution is now recurrent.

	if (step > num_gen) {
		std::cout << "Too few steps to make the state recurrent.." << std::endl;
	} else {
		std::cout << "The sum of all pot numbers is: " << CalcPotSum(*state)+((num_gen-step)*state->size()) << std::endl;
	}

	return 0;
}
