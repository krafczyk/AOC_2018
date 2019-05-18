#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"
#include <signal.h>
#include <limits>

void handler(int s) {
    std::cout << "Caught signal " << s << std::endl;
    exit(1);
}

class IDX {
    public:
        IDX() {
            this->x = 0;
            this->y = 0;
        }
        IDX(int x, int y) {
            this->x = x;
            this->y = y;
        }
        IDX(const IDX& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
        }
        IDX& operator=(const IDX& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            return *this;
        }
        IDX operator+(const IDX& rhs) {
            return IDX(this->x + rhs.x, this->y + rhs.y);
        }
        void show(std::ostream& out) const {
            out << this->x << "," << this->y;
        }
        int x;
        int y;
};

inline int hash(const IDX& idx) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned int A = (idx.x >= 0 ? 2*idx.x : -2*idx.x-1);
    unsigned int B = (idx.y >= 0 ? 2*idx.y : -2*idx.y-1);
    int C = (int)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((idx.x < 0 && idx.y < 0) || (idx.x >= 0 && idx.y >= 0) ? C : -C - 1);
}

const int North = 0;
const int South = 1;
const int East = 2;
const int West = 3;
const int NumDirs = 4;

std::unordered_map<char,IDX> dir_map = {
    {'N', IDX(0,1)},
    {'S', IDX(0,-1)},
    {'E', IDX(1,0)},
    {'W', IDX(-1,0)}
};

int main(int argc, char** argv) {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    int test_value = 0;
    bool test_value_given = false;
    std::string test_expand_regex;
    bool test_expand_regex_given = false;
	ArgParse::ArgParser Parser("Task 39");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-t/--test-val", "Give a test value.", &test_value, ArgParse::Argument::Optional, &test_value_given);
    Parser.AddArgument("-ter", "Test expand regex given", &test_expand_regex, ArgParse::Argument::Optional, &test_expand_regex_given);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    if(test_expand_regex_given) {
        return 0;
    }

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;

    // get regex
    std::string regex_line;
    std::getline(infile, regex_line);

    std::cout << "Passed Regex length: " << regex_line.size() << std::endl;
    if(verbose) {
        std::cout << "Passed Regex: " << regex_line << std::endl;
    }

    struct dist_struct { int value = std::numeric_limits<int>::max(); };

    std::unordered_map<int, dist_struct> dist_map;

    std::vector<IDX> p_stack;

    IDX p(0,0);

    dist_map[hash(p)].value = 0;

    for(int idx = 1; (size_t) idx < regex_line.size()-1; ++idx) {
        char c = regex_line[idx];
        if(c == '(') {
            p_stack.push_back(p);
        } else if (c == '|') {
            p = p_stack[p_stack.size()-1];
        } else if (c == ')') {
            p = p_stack[p_stack.size()-1];
            p_stack.pop_back();
        } else {
            IDX p_next = p+dir_map[c];
            dist_map[hash(p_next)].value = std::min(dist_map[hash(p_next)].value, dist_map[hash(p)].value+1);
            p = p_next;
        }
    }

    int max_dist = 0;
    int num_far = 0;
    auto comp = [&max_dist, &num_far](auto& a) {
        if (a.second.value > max_dist) {
            max_dist = a.second.value;
        }
        if (a.second.value >= 1000) {
            num_far += 1;
        }
    };
    ConstForEach(dist_map, comp);

    std::cout << "Max dist room: " << max_dist << std::endl;
    std::cout << "Num Far rooms: " << num_far << std::endl;

	return 0;
}
