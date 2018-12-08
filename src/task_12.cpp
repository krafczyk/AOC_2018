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

class coordinate {
	public:
		coordinate(int x, int y) {
			this->x = x;
			this->y = y;
		}
		int x;
		int y;
		int dist(const coordinate& rhs) const {
			return abs(this->x-rhs.x)+abs(this->y-rhs.y);
		}
		void print() const {
			std::cout << this->x << ", " << this->y << std::endl;
		}
		bool operator==(const coordinate& rhs) const {
			if ((this->x == rhs.x)&&(this->y == rhs.y)) {
				return true;
			} else {
				return false;
			}
		}
		bool operator!=(const coordinate& rhs) const {
			return !((*this)==rhs);
		}
};

int getClosest(const coordinate& the_coord, const std::vector<coordinate>& coords) {
	int min_dist = std::numeric_limits<int>::max();
	std::vector<int> min_coords;
	for(int coord_idx = 0; coord_idx < (int) coords.size(); ++coord_idx) {
		int dist = the_coord.dist(coords[coord_idx]);
		if (dist < min_dist) {
			min_coords.clear();
			min_coords.push_back(coord_idx);
			min_dist = dist;
		} else if (dist == min_dist) {
			min_coords.push_back(coord_idx);
		}
	}
	if (min_coords.size() > 1) {
		return -1;
	} else {
		return min_coords[0];
	}
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 12");
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

	std::vector<coordinate> coords;

	// Extract coordinates from file
	std::string line;
	while(std::getline(infile, line)) {
		// Find comma
		int comma_idx = -1;
		for(unsigned int idx = 0; idx < line.size(); ++idx) {
			if (line[idx] == ',') {
				comma_idx = idx;
				break;
			}
		}
		int x = std::atoi(line.substr(0,comma_idx).c_str());
		int y = std::atoi(line.substr(comma_idx+2, line.size()-comma_idx-2).c_str());
		coords.push_back(coordinate(x,y));
	}

	if (verbose) {
		std::cout << "Coordinates read in are:" << std::endl;
		for(auto coords_it = coords.begin(); coords_it < coords.end(); ++coords_it) {
			coords_it->print();
		}
	}

	// Determine bounds of coords;
	int min_x = std::numeric_limits<int>::max();
	int max_x = std::numeric_limits<int>::min();
	int min_y = std::numeric_limits<int>::max();
	int max_y = std::numeric_limits<int>::min();

	for(auto coords_it = coords.begin(); coords_it < coords.end(); ++coords_it) {
		if(coords_it->x < min_x) {
			min_x = coords_it->x;
		}
		if(coords_it->x > max_x) {
			max_x = coords_it->x;
		}
		if(coords_it->y < min_y) {
			min_y = coords_it->y;
		}
		if(coords_it->y > max_y) {
			max_y = coords_it->y;
		}
	}

	if (verbose) {
		std::cout << "Bounds are: (" << min_x << ", " << min_y << ", " << max_x << ", " << max_y << ")" << std::endl;
	}

	// Sum distance to all points

	auto get_x = [min_x](int idx) {
		return idx+(min_x-1);
	};
	auto get_y = [min_y](int idx) {
		return idx+(min_y-1);
	};
	auto sum_distances = [&coords](int x, int y) {
		int dist = 0;
		coordinate the_coord(x, y);
		for(auto it = coords.begin(); it != coords.end(); ++it) {
			dist += the_coord.dist(*it);
		}
		return dist;
	};

	// Count up grid points whose distance is less than 10000
	int number_of_points = 0;
	for(int x_idx=0; x_idx < max_x+min_x+1; ++x_idx) {
		for(int y_idx=0; y_idx < max_y+min_y+1; ++y_idx) {
			if(sum_distances(get_x(x_idx),get_y(y_idx)) < 10000) {
				number_of_points += 1;
			}
		}
	}

	std::cout << "The area whose distance sum is less than 10000 has size " << number_of_points << std::endl;

	return 0;
}
