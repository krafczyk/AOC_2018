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

const long erosion_factor = 20183;
long depth;

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

std::unordered_map<long,map_val> emap;

long target_x = 0;
long target_y = 0;
long extra_x = 0;
long extra_y = 0;

long erosion_level(long x, long y) {
    map_val& val = emap[pair_hash_l(x,y)];
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

template<typename Value,typename T>
class priority_queue {
    public:
        priority_queue() {
        }
        void insert(const Value& v, const T& item) {
            // Find where to insert
            auto it = the_queue.begin();
            while((it != the_queue.end())&&(it->first > v)) {
                ++it;
            }
            the_queue.insert(it, std::pair<Value,T>(v,item));
        }
        size_t size() const {
            return the_queue.size();
        }
        bool hasElement(const T& item) const {
            for(auto it=the_queue.begin(); it != the_queue.end(); ++it) {
                if(it->second == item) {
                    return true;
                }
            }
            return false;
        }
        T pop() {
            T answer = the_queue.back;
            the_queue.pop_back();
            return answer;
        }
    private:
        class Compare {
            public:
                bool operator()(const std::pair<Value,T>& a, const std::pair<Value,T>& b) const {
                    return (a.first < b.first);
                }
        };
        std::vector<std::pair<Value,T>> the_queue;
};

class node {
    public:
        node(long x, long y, long mode) {
            this->x = x;
            this->y = y;
            this->mode = mode;
        }
        static const long neither = 0;
        static const long torch = 1;
        static const long gear = 2;
        long x;
        long y;
        long mode;
        long dist_estimate(const node& rhs) const {
            long est = std::abs(this->x-rhs.x)+std::abs(this->y-rhs.y);
            if(rhs.mode != this->mode) {
                est += 7;
            }
            return est;
        }
};

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

    priority_queue<long,node> queue;
    node target(target_x, target_y, node::torch);
    node start(0,0,node::torch);
    queue.insert(start.dist_estimate(target),start);

    //std::cout << "fastest_route: " << fastest_to_target(0, 0, true, false) << std::endl;

	return 0;
}
