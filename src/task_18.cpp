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

class marble {
	public:
		marble(int val) {
			this->val = val;
			this->cw = nullptr;
			this->ccw = nullptr;
		}
		~marble() {
		}
		void print() const {
			std::cout << this->ccw->val << " <- " << this->val << " -> " << this->cw->val << std::endl;
		}
		int val;
		marble* cw;
		marble* ccw;
};

class marble_circle {
	public:
		marble_circle() {
			// Start with an initial marble
			this->current_marble = new marble(0);
			// Connect the marble to itself.
			this->current_marble->ccw = this->current_marble;
			this->current_marble->cw = this->current_marble;

			this->last_marble = 0;
		}
		~marble_circle() {
			marble* init = this->current_marble;
			marble* next = this->current_marble->cw;
			delete this->current_marble;
			this->current_marble = next;
			while(this->current_marble != init) {
				next = this->current_marble->cw;
				delete this->current_marble;
				this->current_marble = next;
			}
		}

		void insertMarbleCW(marble* existing_marble, marble* new_marble) {
				// Nothing special, add between the existing marble and next marble cw from
				// the existing marble.
				marble* next = existing_marble->cw;

				// Between existing and new
				existing_marble->cw = new_marble;
				new_marble->ccw = existing_marble;
				// Between new and next
				new_marble->cw = next;
				next->ccw = new_marble;		
		}

		int removeMarble(marble* the_marble) {
			int val = the_marble->val;
			marble* prev = the_marble->ccw;
			marble* next = the_marble->cw;

			prev->cw = next;
			next->ccw = prev;

			delete the_marble;
			return val;
		}

		// Returns score from adding the marble.
		int addMarble() {
			// Update value of last marble, a bit misleading, but
			// this is now the value of the new marble.
			this->last_marble += 1;
			if (this->last_marble%23 == 0) {
				int answer = this->last_marble;
				answer += removeMarble(this->current_marble->ccw->ccw->ccw->ccw->ccw->ccw->ccw);
				this->current_marble = this->current_marble->ccw->ccw->ccw->ccw->ccw->ccw;
				return answer;
			} else {
				// Nothing special, add between the 1st and 2nd marble cw from
				// the current marble.
				marble* new_marble = new marble(this->last_marble);
				insertMarbleCW(this->current_marble->cw, new_marble);
				// Set a new current marble
				this->current_marble = new_marble;
				return 0;
			}
		}

		void print_state() const {
			marble* init = this->current_marble;
			marble* the_marble = init;
			std::cout << init->val << " ";
			the_marble = the_marble->cw;
			while(the_marble != init) {
				std::cout << the_marble->val << " ";
				the_marble = the_marble->cw;
			}
			std::cout << std::endl;
		}

		void print_detailed_state() const {
			marble* init = this->current_marble;
			marble* the_marble = init;
			the_marble->print();
			the_marble = the_marble->cw;
			while(the_marble != init) {
				the_marble->print();
				the_marble = the_marble->cw;
			}
		}

	private:
		marble* current_marble;
		int last_marble;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	bool test_mode = false;
	ArgParse::ArgParser Parser("Task 17");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
	Parser.AddArgument("-t/--testing", "Indicate whether the answer is available too", &test_mode, ArgParse::Argument::Optional);

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
	int num_players;
	infile >> num_players;

	infile >> line;
	infile >> line;
	infile >> line;
	infile >> line;
	infile >> line;

	int last_val;
	infile >> last_val;

	int test_result = 0;
	if (test_mode) {
		infile >> line;
		infile >> line;
		infile >> line;
		infile >> line;
		infile >> test_result;
	}

	if (verbose) {
		std::cout << num_players << " players; last marble is worth " << last_val << " points" << std::endl;
		if (test_mode) {
			std::cout << "high score should be " << test_result << std::endl;
		}
	}

	marble_circle the_game;
	int scores[num_players];
	for(int idx=0; idx < num_players; ++idx) {
		scores[idx] = 0;
	}
	int num_turns = 0;
	int current_player = 0;
	while(num_turns < last_val) {
		scores[current_player] += the_game.addMarble();
		if(verbose) {
			std::cout << "[" << current_player+1 << "] ";
			the_game.print_state();
			the_game.print_detailed_state();
		}
		current_player += 1;
		current_player = current_player % num_players;
		num_turns += 1;
	}

	int high_score = std::numeric_limits<int>::min();
	for(int idx = 0; idx < num_players; ++idx) {
		if (scores[idx] > high_score) {
			high_score = scores[idx];
		}
	}

	std::cout << num_players << " players; last marble is worth " << last_val << " points" << std::endl;
	std::cout << "The high score is " << high_score << std::endl;

	if (test_mode) {
		if(high_score == test_result) {
			std::cout << "Test Passed!" << std::endl;
			return 0;
		} else {
			std::cout << "Test Failed!" << std::endl;
			return -1;
		}
	}

	return 0;
}
