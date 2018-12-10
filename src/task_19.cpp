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
		void evolve(int seconds = 1) {
			this->x += this->vx*seconds;
			this->y += this->vy*seconds;
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
	int time_sec = 1;
	int w = 80;
	int h = 40;
	ArgParse::ArgParser Parser("Task Template");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
	Parser.AddArgument("-t/--time", "Time to evolve the particles", &time_sec, ArgParse::Argument::Optional);
	Parser.AddArgument("-w/--width", "Specify the width to show", &w, ArgParse::Argument::Optional);
	Parser.AddArgument("-h/--height", "Specify the height to show", &h, ArgParse::Argument::Optional);

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

	// Evolve system specified number of seconds
	for(unsigned int idx=0; idx < particles.size(); ++idx) {
		particles[idx].evolve(time_sec);
	}

	float av_x = 0;
	float av_y = 0;
	int min_x = std::numeric_limits<int>::max();
	int max_x = std::numeric_limits<int>::min();
	int min_y = std::numeric_limits<int>::max();
	int max_y = std::numeric_limits<int>::min();
	for(unsigned int idx=0; idx < particles.size(); ++idx) {
		int x = particles[idx].x;
		int y = particles[idx].y;
		av_x += (float)x;
		av_y += (float)y;
		if(x < min_x) {
				min_x = x;
		}
		if(x > max_x) {
				max_x = x;
		}
		if(y < min_y) {
				min_y = y;
		}
		if(y > max_y) {
				max_y = y;
		}
	}
	av_x = av_x/(float)particles.size();
	av_y = av_y/(float)particles.size();

	if(verbose) {
		std::cout << "Current state" << std::endl;
		for(unsigned int idx=0; idx < particles.size(); ++idx) {
			particles[idx].print();
		}
		std::cout << "Reporting useful stats" << std::endl;
		std::cout << "Min X: " << min_x << std::endl;
		std::cout << "Average X: " << av_x << std::endl;
		std::cout << "Average X: " << (int)av_x << std::endl;
		std::cout << "Max X: " << max_x << std::endl;
		std::cout << "Width: " << max_x-min_x << std::endl;
		std::cout << std::endl;
		std::cout << "Min Y: " << min_y << std::endl;
		std::cout << "Average Y: " << av_y << std::endl;
		std::cout << "Average Y: " << (int)av_y << std::endl;
		std::cout << "Max Y: " << max_y << std::endl;
		std::cout << "Height: " << max_y-min_y << std::endl;
	}

	// Create array to show the results
	bool grid[2*w+1][2*h+1];
	for(int x_idx=0; x_idx < 2*w+1; ++x_idx) {
		for(int y_idx=0; y_idx < 2*h+1; ++y_idx) {
			grid[x_idx][y_idx] = false;
		}
	}

	// write position to index translators
	auto get_x_idx = [w, av_x](int x) {
		if(abs(x-((int)av_x)) > w) {
			return -1;
		}
		return x-((int)av_x)+w;
	};

	auto get_y_idx = [h, av_y](int y) {
		if(abs(y-((int)av_y)) > h) {
			return -1;
		}
		return y-((int)av_y)+h;
	};

	// Mark particle positions
	for(unsigned int idx=0; idx < particles.size(); ++idx) {
		particle& part = particles[idx];
		grid[get_x_idx(part.x)][get_y_idx(part.y)] = true;
	}

	// Print out result!

	for(int y_idx=0; y_idx < 2*h+1; ++y_idx) {
		for(int x_idx=0; x_idx < 2*w+1; ++x_idx) {
			if(grid[x_idx][y_idx]) {
				std::cout << "#";
			} else {
				std::cout << ".";
			}
		}
		std::cout << std::endl;
	}

	return 0;
}
