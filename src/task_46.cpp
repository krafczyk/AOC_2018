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
        bot cross(const bot& rhs) const {
            return bot(this->y*rhs.z-this->z*rhs.y,
                       this->z*rhs.x-this->x*rhs.z,
                       this->x*rhs.y-this->y*rhs.x, 0);
        }
        bot::type dot(const bot& rhs) const {
            return this->x*rhs.x+this->y*rhs.y+this->z*rhs.z;
        }
        double mag() {
            return std::sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
        }
        bool operator==(const bot& rhs) const {
            if((this->x != rhs.x)||(this->y != rhs.y)||
               (this->z != rhs.z)||(this->r != rhs.r)) {
                return false;
            } else {
                return true;
            }
        }
        bool operator!=(const bot& rhs) const {
            return !((*this) == rhs);
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
        bot operator-(const bot& rhs) const {
            return bot(this->x-rhs.x, this->y-rhs.y, this->z-rhs.z, this->r-rhs.r);
        }
        bot operator*(double a) const {
            return bot(this->x*a, this->y*a, this->z*a, this->r*a);
        }
        bot operator/(double a) const {
            return bot(this->x/a, this->y/a, this->z/a, this->r/a);
        }
        bot operator*(bot::type a) const {
            return bot(this->x*a, this->y*a, this->z*a, this->r*a);
        }
        bot operator/(bot::type a) const {
            return bot(this->x/a, this->y/a, this->z/a, this->r/a);
        }
        friend std::ostream& operator<<(std::ostream& out, const bot& b);
        bot::type x;
        bot::type y;
        bot::type z;
        bot::type r;
        bot::type num_int;
};

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

    if(verbose) {
        std::cout << "There are " << bots.size() << " total bots." << std::endl;
    }

	return 0;
}
