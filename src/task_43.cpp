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

class geo_idx {
    public:
        geo_idx() {
            this->value = 0;
            this->set = false;
        }
        geo_idx& operator=(int new_val) {
            this->value = new_val;
            this->set = true;
            return *this;
        }
        int value;
        bool set;
};

std::unordered_map<int, geo_idx> gmap;

int target_x = 0;
int target_y = 0;

int geologic_index(int x, int y) {
    geo_idx& val = gmap[pair_hash(x,y)];
    if(val.set) {
        // Already calculated, return it!
        return val.value;
    }
    // Case 1
    if((x == 0)&&(y==0)) {
        val = 0;
        return val.value;
    }
    // Case 2
    if((x == target_x)&&(y == target_y)) {
        val = 0;
        return val.value;
    }
    // Case 3
    if (y == 0) {
        val = (16807*x)%erosion_factor;
        return val.value;
    }
    // Case 4
    if (x == 0) {
        val = (48271*y)%erosion_factor;
        return val.value;
    }
    // Case 5
    val = (geologic_index(x-1,y)*geologic_index(x,y-1))%erosion_factor;
    return val.value;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    bool Map = false;
	ArgParse::ArgParser Parser("Task 43");
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

    int depth;
    infile.ignore(7);
    infile >> depth;
    infile.ignore(8);
    infile >> target_x;
    infile.ignore(1);
    infile >> target_y;

    infile.close();

    if(verbose) {
        std::cout << "depth: " << depth << std::endl;
        std::cout << "target: " << target_x << "," << target_y << std::endl;
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
                    std::cout << char_map[((geologic_index(x,y)+depth)%erosion_factor)%3];
                }
            }
            std::cout << std::endl;
        }
    }

    int total_risk = 0;
    for(int x = 0; x <= target_x; ++x) {
        for(int y = 0; y <= target_y; ++y) {
            total_risk += ((geologic_index(x,y)+depth)%erosion_factor)%3;
        }
    }

    std::cout << "total_risk: " << total_risk << std::endl;

	return 0;
}
