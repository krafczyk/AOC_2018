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
		}

		// Returns score from adding the marble.
		int addMarble() {
			int new_marble_val = this->last_marble + 1;
			if (new_marble_val%23 == 0) {
			} else {
				// Nothing special, add between the 1st and 2nd marble cw from
				// the current marble.
				marble* new_marble = new marble(new_marble_val);
				marble* first_cw = this->current_marble->cw;
				marble* second_cw = first_cw->cw;

				// Between 1st and new
				first_cw->cw = new_marble;
				new_marble->ccw = first_cw;
				// Between 2nd and new
				new_marble->cw = second_cw;
				second_cw->ccw = new_marble;
				return 0;
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

	return 0;
}
