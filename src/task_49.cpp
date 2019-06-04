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

	std::vector<std::vector<point>> constellations;
	std::vector<point> new_constellation;
	while(points.size() != 0) {
		// Add the first point to the new constellation.
		new_constellation.push_back(points[0]);
		// Remove from points
		points.erase(points.begin());
		size_t num_added = 0;
		do {
			num_added = 0;
			for(auto it = points.begin(); it != points.end();) {
				bool in_range = false;
				for(auto const_it = new_constellation.begin(); const_it != new_constellation.end(); ++const_it) {
					if(it->dist(*const_it) <= 3) {
						in_range = true;
						break;
					}
				}
				if(in_range) {
					// add the point to the constellation
					new_constellation.push_back(*it);
					// remove it from the points
					it = points.erase(it);
					num_added += 1;
				} else {
					++it;
				}
			}
		} while(num_added > 0);
		// Add the new constellation to the list of constellations
		constellations.push_back(new_constellation);
		// Clear the new constellation for another round.
		new_constellation.clear();
	}

	std::cout << "There are " << constellations.size() << " constellations" << std::endl;

	return 0;
}
