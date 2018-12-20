#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <iomanip>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

class point {
	public:
		typedef int p_idx;
		point() {
			this->x = 0;
			this->y = 0;
		}
		point(p_idx x, p_idx y) {
			this->x = x;
			this->y = y;
		}
		point(const point& rhs) = default;
		point& operator=(const point& rhs) = default;
		p_idx get_x() const {
			return this->x;
		}
		p_idx get_y() const {
			return this->y;
		}
		void set_x(p_idx in) {
			this->x = in;
		}
		void set_y(p_idx in ) {
			this->y = in;
		}
		bool operator==(const point& in) const {
			if((this->x == in.x)&&(this->y == in.y)) {
				return true;
			} else {
				return false;
			}
		}
		point operator+(const point& in) const {
			point answer = *this;
			answer.x = in.x;
			answer.y = in.y;
			return answer;
		}
		bool operator<(const point& rhs) const {
			if(this->y < rhs.y) {
				return true;
			} else if (this->y == rhs.y) {
				if(this->x < rhs.x) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
	private:
		p_idx x;
		p_idx y;
		
};

class Range {
	public:
		Range(const point& P1, const point& P2) {
			this->min = point(std::min(P1.get_x(),P2.get_x()),
					  std::min(P1.get_y(),P2.get_y()));
			this->max = point(std::max(P1.get_x(),P2.get_x()),
					  std::max(P1.get_y(),P2.get_y()));
		}
		static bool inline in_extent(point::p_idx low, point::p_idx high, point::p_idx e) {
			if((e <= high)&&(e >= low)) {
				return true;
			} else {
				return false;
			}
		}
		bool contains(const point& p) const {
			if(in_extent(min.get_x(),max.get_x(),p.get_x())&&
			   in_extent(min.get_y(),max.get_y(),p.get_y())) {
				return true;
			} else {
				return false;
			}
		}
		const point& get_min() const {
			return this->min;
		}
		const point& get_max() const {
			return this->max;
		}
	private:
		point min;
		point max;
};

char get_tile(const point& p, const std::vector<Range>& Clay [[maybe_unused]], const std::set<point>& WaterPassed [[maybe_unused]], const std::set<point>& Water [[maybe_unused]]) {
	if(p == point(500,0)) {
		return '+';
	}
	for(auto it = Clay.cbegin(); it != Clay.cend(); ++it) {
		if(it->contains(p)) {
			return '#';
		}
	}
	if(hasElement(WaterPassed, p)) {
		return '|';
	}
	if(hasElement(Water, p)) {
		return '~';
	}
	return '.';
}

void print_state(point::p_idx x_min, point::p_idx x_max, point::p_idx y_max, const std::vector<Range>& Clay, const std::set<point>& WaterPassed, const std::set<point>& Water) {
	// Determine number of digits
	point::p_idx n_digits = ((point::p_idx) std::log10(y_max))+1;

	// Print the rest of the lines
	for(point::p_idx y = 0; y <= y_max; ++y) {
		std::cout << std::setfill(' ') << std::setw(n_digits) << y << " ";
		for(point::p_idx x = x_min; x <= x_max; ++x) {
			std::cout << get_tile(point(x,y), Clay, WaterPassed, Water);
		}
		std::cout << std::endl;
	}
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 33");
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
	std::regex line_regex("([xy])=(\\d+), ([xy])=(\\d+)\\.\\.(\\d+)", std::regex::ECMAScript);
	std::vector<Range> Clay;
	while(std::getline(infile, line)) {
		std::smatch res;
		if(std::regex_match(line, res, line_regex)) {
			if (res[1] == "x") {
				point::p_idx x = std::atoi(res[2].str().c_str());
				point::p_idx y_min = std::atoi(res[4].str().c_str());
				point::p_idx y_max = std::atoi(res[5].str().c_str());
				Clay.push_back(Range(point(x,y_min),point(x,y_max)));
			} else {
				point::p_idx y = std::atoi(res[2].str().c_str());
				point::p_idx x_min = std::atoi(res[4].str().c_str());
				point::p_idx x_max = std::atoi(res[5].str().c_str());
				Clay.push_back(Range(point(x_min,y),point(x_max,y)));
			}
		}
	}

	std::set<point> WaterPassed;
	std::set<point> Water;

	point::p_idx x_min = std::numeric_limits<point::p_idx>::max();
	point::p_idx x_max = std::numeric_limits<point::p_idx>::min();
	point::p_idx y_min = std::numeric_limits<point::p_idx>::max();
	point::p_idx y_max = std::numeric_limits<point::p_idx>::min();
	ConstForEach(Clay, [&](const Range& Rng) {
		if(Rng.get_max().get_x() > x_max) {
			x_max = Rng.get_max().get_x();
		}
		if(Rng.get_min().get_x() < x_min) {
			x_min = Rng.get_min().get_x();
		}
		if(Rng.get_max().get_y() > y_max) {
			y_max = Rng.get_max().get_y();
		}
		if(Rng.get_min().get_y() < y_min) {
			y_min = Rng.get_min().get_y();
		}
	});
	// Widen by one..
	x_min -= 1;
	x_max += 1;
	y_min = 1;

	if(verbose) {
		std::cout << "Initial State" << std::endl;
		print_state(x_min, x_max, y_max, Clay, WaterPassed, Water);
	}



	// Start sim

	// Initial position.
	point::p_idx x = x_min;
	point::p_idx y = 1;

	size_t state [[maybe_unused]] = 0;

	// Not sure what the stopping condition should be.
	bool changed = true;
	while(changed) {
		y = y_min;
		//std::cout << "Round" << std::endl;
		changed = false;
		while(y<=y_max) {
			std::cout << "Row " << y << std::endl;
			x = x_min;
			bool backup = false;
			while(x <=x_max) {
				char current_tile = get_tile(point(x,y), Clay, WaterPassed, Water);
				char left_tile [[maybe_unused]] = get_tile(point(x-1,y), Clay, WaterPassed, Water);
				char right_tile [[maybe_unused]] = get_tile(point(x+1,y), Clay, WaterPassed, Water);
				char up_tile = get_tile(point(x,y-1), Clay, WaterPassed, Water);
				char down_tile [[maybe_unused]] = get_tile(point(x,y+1), Clay, WaterPassed, Water);
				//std::cout << " " << up_tile << " " << std::endl;
				//std::cout << left_tile << current_tile << right_tile << std::endl;
				//std::cout << " " << down_tile << " " << std::endl;
				if(current_tile == '#') {
					// Don't do anything!
					++x;
				} else if(current_tile == '.') {
					// Water falls down always.
					if((up_tile == '+')||(up_tile == '|')) {
						if(!hasElement(WaterPassed, point(x,y))) {
							WaterPassed.insert(point(x,y));
							changed = true;
						}
					}
					++x;
				} else if (current_tile == '|') {
					// Water spreads side to side.
					//std::cout << "here 1" << std::endl;
					if((down_tile == '#')||(down_tile == '~')) {
						//std::cout << "here 2" << std::endl;
						point::p_idx x_left = x;
						bool left_wall = false;
						while(true) {
							char temp_left_tile = get_tile(point(x_left-1,y), Clay, WaterPassed, Water);
							char temp_down_tile = get_tile(point(x_left,y+1), Clay, WaterPassed, Water);
							if(temp_down_tile == '.') {
								break;
							} else if(temp_left_tile == '#') {
								left_wall = true;
								break;
							}
							--x_left;
						}
						point::p_idx x_right = x;
						bool right_wall = false;
						while(true) {
							char temp_right_tile = get_tile(point(x_right+1,y), Clay, WaterPassed, Water);
							char temp_down_tile = get_tile(point(x_right,y+1), Clay, WaterPassed, Water);
							if(temp_down_tile == '.') {
								break;
							} else if(temp_right_tile == '#') {
								right_wall = true;
								break;
							}
							++x_right;
						}
						bool fill_water = false;
						if(left_wall&&right_wall) {
							fill_water = true;
						}
						// Fill characters in.
						for(point::p_idx temp_x = x_left; temp_x <= x_right; ++temp_x) {
							char the_tile = get_tile(point(temp_x, y), Clay, WaterPassed, Water);
							if((the_tile == '|')&&(fill_water)) {
								removeFirst(WaterPassed, point(temp_x,y));
								changed = true;
							}
							if((the_tile == '~')||(the_tile == '#')) {
								throw std::runtime_error("This shouldn't happen!");
							}
							if(fill_water) {
								Water.insert(point(temp_x,y));
								changed = true;
							} else {
								if(the_tile == '.') {
									WaterPassed.insert(point(temp_x,y));
									changed = true;
								} else if(the_tile != '|') {
									throw std::runtime_error("This shouldn't happen either");
								}
							}
						}
						// Advance.
						backup = true;
						x = x_right;
					} else {
						++x;
					}
				} else if (current_tile == '~') {
					// Don't do anything
					++x;
				}
			}
			if(backup) {
				std::cout << "Backup set" << std::endl;
				--y;
			} else {
				++y;
			}
		}
		std::cout << "New State" << std::endl;
		print_state(x_min, x_max, y_max, Clay, WaterPassed, Water);
	}

	std::cout << "Current State" << std::endl;
	print_state(x_min, x_max, y_max, Clay, WaterPassed, Water);
	return 0;
}
