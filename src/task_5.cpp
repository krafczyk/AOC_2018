#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	ArgParse::ArgParser Parser("Task 5");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);

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
	//#3 @ 5,5: 2x2
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
		infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "#" << id << " @ " << x << "," << y << ": " << w << "x" << h << std::endl;
		for(int i=0; i<w; ++i) {
			for(int j=0; j<h; ++j) {
				fabric[i+x][j+y] += 1;
			}
		}
	}

	int number_overused = 0;
	for(int i=0; i<1000; ++i) {
		for(int j=0; j< 1000; ++j) {
			if (fabric[i][j] > 1) {
				number_overused += 1;
			}
		}
	}

	std::cout << "Amount overused: " << number_overused << std::endl;

	return 0;
}
