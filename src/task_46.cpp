#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include "ArgParseStandalone.h"
#include "utilities.h"

class bot {
    public:
        typedef long type;
        bot() {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->r = 0;
        }
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
        bool operator==(const bot& rhs) const {
            if((this->x != rhs.x)||(this->y != rhs.y)||
               (this->z != rhs.z)||(this->r != rhs.r)) {
                return false;
            } else {
                return true;
            }
        }
        bot::type sum() const {
            return this->x+this->y+this->z;
        }
        int dist(const bot& rhs) const {
            int answer = 0;
            answer += std::abs(this->x-rhs.x);
            answer += std::abs(this->y-rhs.y);
            answer += std::abs(this->z-rhs.z);
            return answer;
        }
        bot operator+(const bot& rhs) const {
            return bot(this->x+rhs.x, this->y+rhs.y, this->z+rhs.z, this->r+rhs.r);
        }
        friend std::ostream& operator<<(std::ostream& out, const bot& b);
        bot::type x;
        bot::type y;
        bot::type z;
        bot::type r;
        bot::type num_int;
};

std::unordered_map<int,bot> dirs = {
    {0, bot(1,0,0,0)},
    {1, bot(0,1,0,0)},
    {2, bot(0,0,1,0)},
    {3, bot(1,1,0,0)},
    {4, bot(1,-1,0,0)},
    {5, bot(0,1,1,0)},
    {6, bot(0,1,-1,0)},
    {7, bot(1,0,1,0)},
    {8, bot(1,0,-1,0)},
    {9, bot(1,1,1,0)},
    {10, bot(-1,1,1,0)},
    {11, bot(1,-1,1,0)},
    {12, bot(1,1,-1,0)},
    {13, bot(1,-1,-1,0)},
    {14, bot(-1,1,-1,0)},
    {15, bot(-1,-1,1,0)},
};

const int num_dirs = 16;

std::ostream& operator<<(std::ostream& out, const bot& b) {
    out << "pos=<";
    out << b.x << ",";
    out << b.y << ",";
    out << b.z << ">, r=";
    out << b.r;
    return out;
}


int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false; ArgParse::ArgParser Parser("Task 46");
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

    auto num_intersections = [&](const bot& point) {
        int total = 0;
        std::for_each(bots.begin(), bots.end(), [&](const bot& b) {
            if(b.dist(point) <= b.r) {
               total += 1;
            }
        });
        return total;
    };

    bot::type min_dist = std::numeric_limits<bot::type>::max();
    bot destination(0,0,0,0);
    bot current_position = destination;
    int max_int = num_intersections(destination);
    while(true) {
        bot neighbor_candidate = current_position;
        for(int d = 0; d < num_dirs; ++d) {
            // Forward
            // Backward
        }
    }

    std::cout << "The closest point in the high signal region is " << min_dist << " units away!" << std::endl;

	return 0;
}
