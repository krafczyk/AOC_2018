#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>

class elf_claim {
	public:
		elf_claim(int id, int x, int y, int w, int h) {
			this->id = id;
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}
		int id;
		int x;
		int y;
		int w;
		int h;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 5");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	//open box id file
	std::ifstream infile(input_filepath.c_str());

	// Initialize fabric
	int fabric[1000][1000];
	for (int i=0; i < 1000; ++i) {
		for(int j=0; j < 1000; ++j) {
			fabric[i][j] = 0;
		}
	}

	// List of claims
	std::vector<elf_claim> claims;
	int k = 0;
	while(!infile.eof()) {
		++k;
		char temp;
		// Dumb C++ implementations...
		if (!infile.get(temp)) {
			break;
		}
		std::string temp_str;
		getline(infile, temp_str, ' ');
		int id = std::atoi(temp_str.c_str());
		infile.get(temp);
		infile.get(temp);
		getline(infile, temp_str, ',');
		int x = std::atoi(temp_str.c_str());
		int y;
		infile >> y;
		infile.get(temp);
		infile.get(temp);
		getline(infile, temp_str, 'x');
		int w = std::atoi(temp_str.c_str());
		int h;
		infile >> h;
		claims.push_back(elf_claim(id, x, y, w, h));
		infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (verbose) {
			std::cout << "#" << id << " @ " << x << "," << y << ": " << w << "x" << h << std::endl;
		}
		for(int i=0; i<w; ++i) {
			for(int j=0; j<h; ++j) {
				fabric[i+x][j+y] += 1;
			}
		}
	}

	for(unsigned int claim_id=0; claim_id < claims.size(); ++claim_id) {
		elf_claim& claim = claims[claim_id];
		bool clean = true;
		for(int i=0; i < claim.w; ++i) {
			for(int j=0; j < claim.h; ++j) {
				if (fabric[claim.x+i][claim.y+j] > 1) {
					clean = false;
					break;
				}
			}
			if (!clean) {
				break;
			}
		}
		if (clean) {
			std::cout << "Claim " << claim.id << " is clean!" << std::endl;
			return 0;
		}
	}

	return 0;
}
