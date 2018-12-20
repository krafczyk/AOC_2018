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
	private:
		p_idx x;
		p_idx y;
		
};

class Range {
	public:
		//Range(const std::string& line) {
		//	bool x_mode = true;
		//	if(line[0] == 'y') {
		//		x_mode = false;
		//	}
		//}
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

void print_state(const std::vector<Range>& Clay) {
	point::p_idx fountain_x = 500;
	// Determine min and max x and y.
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

	// Determine number of digits
	point::p_idx n_digits = ((point::p_idx) std::log10(y_max))+1;

	auto x_from_idx = [&](point::p_idx idx) {
		return x_min+idx;
	};
	auto y_from_idx [[maybe_unused]] = [&](point::p_idx idx) {
		return y_min+idx;
	};

	// Print top line
	std::cout << std::setfill(' ') << std::setw(n_digits) << 0 << " ";
	for(point::p_idx idx = 0; idx < x_max-x_min+1; ++idx) {
		if(x_from_idx(idx) == fountain_x) {
			std::cout << '+';
		} else {
			std::cout << '.';
		}
	}
	std::cout << std::endl;

	// Print the rest of the lines
	for(point::p_idx y = 1; y <= y_max; ++y) {
		std::cout << std::setfill(' ') << std::setw(n_digits) << y << " ";
		for(point::p_idx x = x_min; x <= x_max; ++x) {
			bool is_clay = false;
			for(auto it = Clay.cbegin(); it != Clay.cend(); ++it) {
				if(it->contains(point(x,y))) {
					is_clay = true;
					break;
				}
			}
			if(is_clay) {
				std::cout << '#';
			} else {
				std::cout << '.';
			}
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

	print_state(Clay);

	return 0;
}
