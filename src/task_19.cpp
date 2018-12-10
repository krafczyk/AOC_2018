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

class particle {
	public:
		particle(int x, int y, int vx, int vy) {
			this->x = x;
			this->y = y;
			this->vx = vx;
			this->vy = vy;
		}
		void evolve() {
			this->x += this->vx;
			this->y += this->vy;
		}
		void print() const {
			std::cout << "position=<" << this->x << ", " << this->y << "> velocity=<" << this->vx << ", " << this->vy << ">" << std::endl;
		}
		int x;
		int y;
		int vx;
		int vy;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task Template");
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

	std::vector<particle> particles;
	std::string line;
	while(std::getline(infile, line)) {
		// find markers of interest
		int f_langle = -1;
		int f_comma = -1;
		int f_rangle = -1;
		int s_langle = -1;
		int s_comma = -1;
		int s_rangle = -1;
		int idx = 0;
		while(line[idx] != '<') {
			++idx;
		}
		f_langle = idx;
		while(line[idx] != ',') {
			++idx;
		}
		f_comma = idx;
		while(line[idx] != '>') {
			++idx;
		}
		f_rangle = idx;
		while(line[idx] != '<') {
			++idx;
		}
		s_langle = idx;
		while(line[idx] != ',') {
			++idx;
		}
		s_comma = idx;
		while(line[idx] != '>') {
			++idx;
		}
		s_rangle = idx;
		int x = std::atoi(line.substr(f_langle+1,f_comma-f_langle-1).c_str());
		int y = std::atoi(line.substr(f_comma+1,f_rangle-f_comma-1).c_str());
		int vx = std::atoi(line.substr(s_langle+1,s_comma-s_langle-1).c_str());
		int vy = std::atoi(line.substr(s_comma+1,s_rangle-s_comma-1).c_str());

		particles.push_back(particle(x, y, vx, vy));
	}

	if (verbose) {
		std::cout << "Particles read in:" << std::endl;
		for(unsigned int idx = 0; idx < particles.size(); ++idx) {
			particles[idx].print();
		}
	}

	return 0;
}
