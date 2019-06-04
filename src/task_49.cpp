#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef int type;

class point {
	public:
		point(type x, type y, type z, type t) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->t = t;
		}
		point(const point& rhs) {
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;
			this->t = rhs.t;
		}
		type dist(const point& rhs) const {
			return std::abs(this->x-rhs.x)+
                   std::abs(this->y-rhs.y)+
                   std::abs(this->z-rhs.z)+
                   std::abs(this->t-rhs.t);
		}
		friend std::ostream& operator<<(std::ostream& out, const point& p);
	private:
		type x;
		type y;
		type z;
		type t;
};

std::ostream& operator<<(std::ostream& out, const point& p) {
	out << p.x << "," << p.y << "," << p.z << "," << p.t;
	return out;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 49");
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

	std::vector<point> points;
	std::regex point_matcher("^([\\-0-9]*),([\\-0-9]*),([\\-0-9]*),([\\-0-9]*)$", std::regex::ECMAScript);
	std::string line;
    auto fetch_value = [](std::string str) {
		type val;
        std::istringstream iss(str);
        iss >> val;
		return val;
    };
	while(std::getline(infile, line)) {
		std::smatch match;
		if(!std::regex_match(line, match, point_matcher)) {
			std::cerr << "There was a problem matching a point!" << std::endl;
		}
		points.push_back(point(fetch_value(match[1].str()),
                               fetch_value(match[2].str()),
                               fetch_value(match[3].str()),
                               fetch_value(match[4].str())));
    }

	if(verbose) {
		std::cout << "Got the following points: " << std::endl;
		ForEach(points, [](const point& p) {
			std::cout << p << std::endl;
		});
	}
	return 0;
}
