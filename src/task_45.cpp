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

class bot {
    public:
        bot(int x, int y, int z, int r) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->r = r;
        }
        bot(const bot& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->r = rhs.r;
        }
        bot& operator=(const bot& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->r = rhs.r;
            return (*this);
        }
        int dist(const bot& rhs) const {
            int answer = 0;
            answer += std::abs(this->x-rhs.x);
            answer += std::abs(this->y-rhs.y);
            answer += std::abs(this->z-rhs.z);
            return answer;
        }
        void show() const {
            std::cout << "pos=<";
            std::cout << this->x << ",";
            std::cout << this->y << ",";
            std::cout << this->z << ">, r=";
            std::cout << this->r << std::endl;
        }
        int x;
        int y;
        int z;
        int r;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 45");
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

    std::vector<bot> bots;

    int x = 0;
    int y = 0;
    int z = 0;
    int r = 0;
    std::string line;
    while(std::getline(infile, line)) {
        std::istringstream iss(line);
        iss.ignore(5);
        iss >> x;
        iss.ignore(1);
        iss >> y;
        iss.ignore(1);
        iss >> z;
        iss.ignore(5);
        iss >> r;
        bots.push_back(bot(x,y,z,r));
    }

    if(verbose) {
        std::for_each(bots.begin(), bots.end(), [](const bot& a) {
            a.show();
        });
    }

    auto strongest_bot_it = bots.begin();
    auto it = strongest_bot_it+1;
    while(it != bots.end()) {
        if(it->r > strongest_bot_it->r) {
            strongest_bot_it = it;
        }
        ++it;
    }
    int num_bots = 0;
    for(it = bots.begin(); it != bots.end(); ++it) {
        if(strongest_bot_it->dist(*it) <= strongest_bot_it->r) {
            num_bots += 1;
        }
    }

    std::cout << "There are " << num_bots << " within range of the strongest bot." << std::endl;

	return 0;
}
