#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "ArgParseStandalone.h"
#include "utilities.h"

const int erosion_factor = 20183;
int depth;

class map_val {
    public:
        map_val() {
            this->value = 0;
            this->set = false;
        }
        map_val& operator=(int new_val) {
            this->value = new_val;
            this->set = true;
            return *this;
        }
        int value;
        bool set;
};

std::unordered_map<int, map_val> emap;

int target_x = 0;
int target_y = 0;
int extra_x = 0;
int extra_y = 0;

int erosion_level(int x, int y) {
    map_val& val = emap[pair_hash(x,y)];
    if(val.set) {
        // Already calculated, return it!
        return val.value;
    }
    // Case 1
    if((x == 0)&&(y==0)) {
        val = (depth)%erosion_factor;
        return val.value;
    }
    // Case 2
    if((x == target_x)&&(y == target_y)) {
        val = (depth)%erosion_factor;
        return val.value;
    }
    // Case 3
    if (y == 0) {
        val = ((16807*x)+depth)%erosion_factor;
        return val.value;
    }
    // Case 4
    if (x == 0) {
        val = ((48271*y)+depth)%erosion_factor;
        return val.value;
    }
    // Case 5
    val = (((erosion_level(x-1,y)*erosion_level(x,y-1))%erosion_factor)+depth)%erosion_factor;
    return val.value;
}

std::unordered_map<int, map_val> fastest_to_target_neither;
std::unordered_map<int, map_val> fastest_to_target_torch;
std::unordered_map<int, map_val> fastest_to_target_gear;

void advance_positions(int& x, int& y, int dir) {
    if(dir == 0) {
        // North
        y -= 1;
    } else if (dir == 1) {
        // South
        y += 1;
    } else if (dir == 2) {
        // East
        x += 1;
    } else {
        // West
        x -= 1;
    }
}

int fastest_to_target(int x, int y, bool torch, bool gear) {
    // Detect solid rock wall.
    if((x < 0)||(y < 0)) {
        return std::numeric_limits<int>::max();
    }
    if((x > extra_x)||(y > extra_y)) {
        return std::numeric_limits<int>::max();
    }
    map_val* pval;
    if(torch) {
        // We have the torch currently equipped.
        pval = &(fastest_to_target_torch[pair_hash(x,y)]);
        if(pval->set) {
            return pval->value;
        }
        if((x == target_x)&&(y == target_y)) {
            // We've arrived!
            return 0;
        }
        std::vector<int> dists;
        for(int d = 0; d < 4; ++d) {
            if(d == 0) {
                // Skip north..
                continue;
            }
            if(d == 4) {
                // Skip west..
                continue;
            }
            int nx = x;
            int ny = y;
            advance_positions(nx, ny, d);
            int neighbor_type = erosion_level(nx, ny)%3;
            if(neighbor_type == 0) {
                // Rock
                // Don't change equipment
                dists.push_back(fastest_to_target(nx, ny, true, false)+1);
                // Change to gear
                dists.push_back(fastest_to_target(nx, ny, false, true)+1+7);
            } else if (neighbor_type == 1) {
                // Wet
                // Change to gear
                dists.push_back(fastest_to_target(nx, ny, false, true)+1+7);
                // Change to neither
                dists.push_back(fastest_to_target(nx, ny, false, false)+1+7);
            } else {
                // Narrow
                // Don't change equipment
                dists.push_back(fastest_to_target(nx, ny, true, false)+1);
                // Change to neither
                dists.push_back(fastest_to_target(nx, ny, false, false)+1+7);
            }
        }
        *pval = *std::min_element(dists.begin(), dists.end());
        return pval->value;
    } else if (gear) {
        // We have the climbing gear currently equipped.
        pval = &(fastest_to_target_gear[pair_hash(x,y)]);
        if(pval->set) {
            return pval->value;
        }
        if((x == target_x)&&(y == target_y)) {
            // We need to change gears
            return 7;
        }
        std::vector<int> dists;
        for(int d = 0; d < 4; ++d) {
            if(d == 0) {
                // Skip north..
                continue;
            }
            if(d == 4) {
                // Skip west..
                continue;
            }
            int nx = x;
            int ny = y;
            advance_positions(nx, ny, d);
            int neighbor_type = erosion_level(nx, ny)%3;
            if(neighbor_type == 0) {
                // Rock
                // Change to torch
                dists.push_back(fastest_to_target(nx, ny, true, false)+1+7);
                // Don't change equipment
                dists.push_back(fastest_to_target(nx, ny, false, true)+1);
            } else if (neighbor_type == 1) {
                // Wet
                // Don't change equipment
                dists.push_back(fastest_to_target(nx, ny, false, true)+1);
                // Change to neither
                dists.push_back(fastest_to_target(nx, ny, false, false)+1+7);
            } else {
                // Narrow
                // Change to torch
                dists.push_back(fastest_to_target(nx, ny, true, false)+1+7);
                // Change to neither
                dists.push_back(fastest_to_target(nx, ny, false, false)+1+7);
            }
        }
        *pval = *std::min_element(dists.begin(), dists.end());
        return pval->value;
    } else {
        // We have no gear currently equipped.
        pval = &(fastest_to_target_neither[pair_hash(x,y)]);
        if(pval->set) {
            return pval->value;
        }
        if((x == target_x)&&(y == target_y)) {
            // We need to change gears
            return 7;
        }
        std::vector<int> dists;
        for(int d = 0; d < 4; ++d) {
            if(d == 0) {
                // Skip north..
                continue;
            }
            if(d == 4) {
                // Skip west..
                continue;
            }
            int nx = x;
            int ny = y;
            advance_positions(nx, ny, d);
            int neighbor_type = erosion_level(nx, ny)%3;
            if(neighbor_type == 0) {
                // Rock
                // Change to torch
                dists.push_back(fastest_to_target(nx, ny, true, false)+1+7);
                // Change to gear
                dists.push_back(fastest_to_target(nx, ny, false, true)+1+7);
            } else if (neighbor_type == 1) {
                // Wet
                // Change to gear
                dists.push_back(fastest_to_target(nx, ny, false, true)+1+7);
                // Proceed with neither
                dists.push_back(fastest_to_target(nx, ny, false, false)+1);
            } else {
                // Narrow
                // Change to torch
                dists.push_back(fastest_to_target(nx, ny, true, false)+1+7);
                // Proceed with neither
                dists.push_back(fastest_to_target(nx, ny, false, false)+1);
            }
        }
        *pval = *std::min_element(dists.begin(), dists.end());
        return pval->value;
    }
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    bool Map = false;
	ArgParse::ArgParser Parser("Task 44");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-m/--map", "Print map", &Map);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);

    infile.ignore(7);
    infile >> depth;
    infile.ignore(8);
    infile >> target_x;
    infile.ignore(1);
    infile >> target_y;
    infile.ignore(7);
    infile >> extra_x;
    infile.ignore(1);
    infile >> extra_y;

    infile.close();

    if(verbose) {
        std::cout << "depth: " << depth << std::endl;
        std::cout << "target: " << target_x << "," << target_y << std::endl;
        std::cout << "extra: " << extra_x << "," << extra_y << std::endl;
    }

    std::unordered_map<int, char> char_map = {
        { 0, '.' },
        { 1, '=' },
        { 2, '|' }
    };

    if (Map) {
        for(int y = 0; y <= target_y; ++y) {
            for(int x = 0; x <= target_x; ++x) {
                if((x==0)&&(y==0)) {
                    std::cout << "M";
                } else if ((x==target_x)&&(y==target_y)) {
                    std::cout << "T";
                } else {
                    std::cout << char_map[erosion_level(x,y)%3];
                }
            }
            std::cout << std::endl;
        }
    }

    std::cout << "fastest_route: " << fastest_to_target(0, 0, true, false) << std::endl;

	return 0;
}
