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
		Range(const point& P1, const point& P2) {
			this->p1 = point(P1);
			this->p2 = point(P2);
		}
		static bool in_extent(point::p_idx e1, point::p_idx e2, point::p_idx e) {
			if((e <= std::max(e1,e2))&&(e >= std::min(e1,e2))) {
				return true;
			} else {
				return false;
			}
		}
		bool is_in(const point& p) const {
			if(in_extent(p1.get_x(),p2.get_x())
		}
	private:
		point p1;
		point p2;
};

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

	return 0;
}
