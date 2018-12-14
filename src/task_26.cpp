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

class track_map {
	public:
		track_map(size_t num_lines, size_t line_size) {
			map = new char[num_lines*line_size];
			this->num_lines = num_lines;
			this->line_size = line_size;

		}
		~track_map() {
			delete [] map;
		}
		char operator()(size_t line_idx, size_t idx) const{
			return map[line_idx*line_size+idx];
		}
		char& assign(size_t line_idx, size_t idx) {
			return map[line_idx*line_size+idx];
		}
	private:
		size_t num_lines;
		size_t line_size;
		char* map;
};

class cart {
	public:
		cart(size_t line, size_t x, char state, size_t id) {
			this->line = line;
			this->x = x;
			set_direction(state);
			this->turn_state = 0;
			this->id = id;
		}
		bool operator<(const cart& rhs) const {
			if (this->line < rhs.line) {
				return true;
			} else if (this->line == rhs.line) {
				if(this->x < rhs.x) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
		bool operator==(const cart& rhs) const {
			if((this->line == rhs.line)&&(this->x == rhs.x)) {
				return true;
			} else {
				return false;
			}
		}
		bool operator!=(const cart& rhs) const {
			return (!((*this)==rhs));
		}
		size_t get_line() const {
			return this->line;
		}
		size_t get_x() const {
			return this->x;
		}
		size_t get_id() const {
			return this->id;
		}
		void advance(const track_map& map) {
			// Move cart on track.
			switch(this->direction) {
				case (2): {
					this->x -= 1;
					break;
				}
				case (0): {
					this->x += 1;
					break;
				}
				case (1): {
					this->line -= 1;
					break;
				}
				case (3): {
					this->line += 1;
					break;
				}
				default: {
					std::cerr << "Error! unhandled state!!" << std::endl;
				}
			}
			// Turn cart based on track
			char track_tile = map(this->line,this->x);
			if(track_tile == '/') {
				if((this->direction == 0)||(this->direction == 2)) {
					this->turn_ccw();
				} else if ((this->direction == 1)||(this->direction == 3)) {
					this->turn_cw();
				} else {
					std::cerr << "Error! unesxpected direction!!" << std::endl;
				}
			} else if(track_tile == '\\') {
				if((this->direction == 0)||(this->direction == 2)) {
					this->turn_cw();
				} else if ((this->direction == 1)||(this->direction == 3)) {
					this->turn_ccw();
				} else {
					std::cerr << "Error! unexpected direction!!" << std::endl;
				}
			} else if(track_tile == '+') {
				if(this->turn_state == 0) {
					this->turn_ccw();
					this->advance_turn();
				} else if (this->turn_state == 1) {
					this->advance_turn();
				} else if (this->turn_state == 2) {
					this->turn_cw();
					this->advance_turn();
				} } else if(!((track_tile == '-')||(track_tile == '|'))) {
				// Do nothing if the track is straight.
				// Catch strange tracks
				std::cerr << "Error! unexpected track!!" << std::endl;
			}
		}
		// Track cart turns
		void advance_turn() {
			this->turn_state += 1;
			this->turn_state = this->turn_state % 3;
		}
		// Helper function to set the cart direction
		void set_direction(char state) {
			if(state == '>') {
				this->direction = 0;
			} else if (state == '^') {
				this->direction = 1;
			} else if (state == '<') {
				this->direction = 2;
			} else if (state == 'v') {
				this->direction = 3;
			}
		}
		// Get the cart state
		char get_state() const {
			if(this->direction == 0) {
				return '>';
			} else if (this->direction == 1) {
				return '^';
			} else if (this->direction == 2) {
				return '<';
			} else {
				return 'v';
			}
		}
		// Turn management
		void turn_cw() {
			this->direction -= 1;
			normalize_direction();
		}
		void turn_ccw() {
			this->direction += 1;
			normalize_direction();
		}
		void normalize_direction() {
			if(this->direction < 0) {
				this->direction += 4;
			}
			this->direction = this->direction%4;
		}
	private:
		size_t id;
		size_t line;
		size_t x;
		int turn_state;
		int direction;
};

size_t carts_at_location(size_t line, size_t x, const std::vector<cart>& carts) {
	size_t num_carts = 0;
	for(size_t idx = 0; idx < carts.size(); ++idx) {
		if((carts[idx].get_line() == line)&&(carts[idx].get_x() == x)) {
			num_carts += 1;
		}
	}
	return num_carts;
}

size_t get_cart_idx(size_t line, size_t x, const std::vector<cart>& carts) {
	for(size_t idx = 0; idx < carts.size(); ++idx) {
		if((carts[idx].get_line() == line)&&(carts[idx].get_x() == x)) {
			return idx;
		}
	}
	return std::numeric_limits<size_t>::max();
}

void print_state(const track_map& map, size_t num_lines, size_t line_size, const std::vector<cart>& carts) {
	for(size_t line_idx = 0; line_idx < num_lines; ++line_idx) {
		for(size_t idx = 0; idx < line_size; ++idx) {
			char the_char = 0;
			// Check whether there are any carts at this location
			size_t num_carts = carts_at_location(line_idx, idx, carts);
			if(num_carts > 0) {
				if(num_carts > 1) {
					the_char = 'X';
				} else {
					the_char = carts[get_cart_idx(line_idx, idx, carts)].get_state();
				}
			} else {
				the_char = map(line_idx,idx);
			}
			std::cout << the_char;
		}
		std::cout << std::endl;
	}
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	std::vector<size_t> print_steps;
	size_t max_steps = std::numeric_limits<size_t>::max();
	ArgParse::ArgParser Parser("Task 26");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-p/--print-steps", "vector of steps to print", &print_steps, ArgParse::Argument::Optional);
	Parser.AddArgument("-m/--max-steps", "Maximum number of steps to execute", &max_steps, ArgParse::Argument::Optional);
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

	std::vector<std::string> input_data;
	std::string line;
	while(std::getline(infile, line)) {
		input_data.push_back(line);
	}

	// Check whether all lines are the same length
	size_t line_size = input_data[0].size();
	bool size_problem = false;
	size_t num_lines = input_data.size();
	for(size_t idx=0; idx < num_lines; ++idx) {
		if(input_data[idx].size() != line_size) {
			size_problem = true;
			break;
		}
	}
	if(size_problem) {
		std::cerr << "Not all lines have identical length" << std::endl;
		return -1;
	}

	// Create track map and get carts.
	track_map map(num_lines, line_size);
	std::vector<cart> carts;
	size_t cart_counter = 0;
	for(size_t line_idx = 0; line_idx < num_lines; ++line_idx) {
		for(size_t idx = 0; idx < line_size; ++idx) {
			char the_char = input_data[line_idx][idx];
			// Assumption: the cart is always on a straightaway. This was true for my puzzle input.
			char cart_char = 0;
			bool have_cart = false;
			if((the_char == '^')||(the_char == 'v')) {
				cart_char = the_char;
				have_cart = true;
				the_char = '|';
			}
			if((the_char == '>')||(the_char == '<')) {
				cart_char = the_char;
				have_cart = true;
				the_char = '-';
			}
			if(have_cart) {
				if(cart_char == 0) {
					std::cerr << "Issue creating cart!!" << std::endl;
					return -1;
				}
				carts.push_back(cart(line_idx, idx, cart_char, cart_counter));
				++cart_counter;
			}
			map.assign(line_idx,idx) = the_char;
		}
	}

	if(verbose) {
		std::cout << "Initial state:" << std::endl;
		print_state(map, num_lines, line_size, carts);
	}

	size_t steps = 0;
	while((carts.size() > 1)&&(steps < max_steps)) {
		// Sort the carts
		std::sort(carts.begin(), carts.end());

		// Loop over carts
		auto cart_it = carts.begin();
		while(cart_it != carts.end()) {
			// Advance the cart
			cart_it->advance(map);
			// Check for collisions, and remove offending carts.
			auto second_cart_it = carts.begin();
			bool collision = false;
			while(second_cart_it != carts.end()) {
				if(second_cart_it->get_id() == cart_it->get_id()) {
					// skip the current cart.
					++second_cart_it;
					continue;
				} else {
					if(*second_cart_it == *cart_it) {
						// Remove collided cart
						collision = true;
						if(second_cart_it < cart_it) {
							// swap the iterators to avoid invalidating
							std::swap(cart_it, second_cart_it);
						}
						second_cart_it = carts.erase(second_cart_it);
					} else {
						// Advance normally
						++second_cart_it;
					}
				}
			}
			if (collision) {
				// Remove yourself.
				cart_it = carts.erase(cart_it);
			} else {
				// Advance normally
				++cart_it;
			}
		}

		++steps;
	}

	if(verbose) {
		std::cout << "Final state" << std::endl;
		print_state(map, num_lines, line_size, carts);
	}

	// Print state
	if(carts.size() == 1) {
		std::cout << "Last cart detected at " << carts[0].get_x() << "," << carts[0].get_line() << " on step " << steps+1 << std::endl;
	}

	return 0;
}
