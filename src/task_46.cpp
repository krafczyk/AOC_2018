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
        typedef long type;
        bot(type x, type y, type z, type r) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->r = r;
            this->num_int = 0;
        }
        bot(const bot& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->r = rhs.r;
            this->num_int = rhs.num_int;
        }
        bot& operator=(const bot& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->z = rhs.z;
            this->r = rhs.r;
            this->num_int = rhs.num_int;
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
        bot::type x;
        bot::type y;
        bot::type z;
        bot::type r;
        bot::type num_int;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 46");
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

    bot::type x = 0;
    bot::type y = 0;
    bot::type z = 0;
    bot::type r = 0;
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
        std::cout << "There are " << bots.size() << " total bots." << std::endl;
    }

    // Find extent.
    bot::type min_x = std::numeric_limits<bot::type>::max();
    bot::type max_x = std::numeric_limits<bot::type>::min();
    bot::type min_y = std::numeric_limits<bot::type>::max();
    bot::type max_y = std::numeric_limits<bot::type>::min();
    bot::type min_z = std::numeric_limits<bot::type>::max();
    bot::type max_z = std::numeric_limits<bot::type>::min();
    bot::type min_r = std::numeric_limits<bot::type>::max();
    bot::type max_r = std::numeric_limits<bot::type>::min();
 
    std::for_each(bots.begin(), bots.end(), [&](const bot& b) {
        if(b.x < min_x) {
            min_x = b.x;
        }
        if(b.x > max_x) {
            max_x = b.x;
        }
        if(b.y < min_y) {
            min_y = b.y;
        }
        if(b.y > max_y) {
            max_y = b.y;
        }
        if(b.z < min_z) {
            min_z = b.z;
        }
        if(b.z > max_z) {
            max_z = b.z;
        }
        if(b.r < min_r) {
            min_r = b.r;
        }
        if(b.r > max_r) {
            max_r = b.r;
        }
    });

    if(verbose) {
        std::cout << "X range: [" << min_x << "," << max_x << "]" << std::endl;
        std::cout << "Y range: [" << min_y << "," << max_y << "]" << std::endl;
        std::cout << "Z range: [" << min_z << "," << max_z << "]" << std::endl;
        std::cout << "R range: [" << min_r << "," << max_r << "]" << std::endl;
    }

    // Initially all num_int counters are 0.
    for(auto it_1 = bots.begin(); it_1 != bots.end(); ++it_1) {
        for(auto it_2 = it_1+1; it_2 != bots.end(); ++it_2) {
            if(it_1->dist(*it_2) <= (it_1->r + it_2->r)) {
                it_1->num_int += 1;
                it_2->num_int += 1;
            }
        }
    }

    bot::type max_int = 0;
    std::for_each(bots.begin(), bots.end(), [&](const bot& b) {
        if(b.num_int > max_int) {
            max_int = b.num_int;
        }
    });

    std::cout << max_int << " is the most number of bots whose range intersects." << std::endl;

	return 0;
}
