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

void handler(int s) {
    std::cout << "Caught signal " << s << std::endl;
    exit(1);
}

typedef std::pair<int,int> IDX;

inline int hash(IDX& idx) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned int A = (idx.first >= 0 ? 2*idx.first : -2*idx.first-1);
    unsigned int B = (idx.second >= 0 ? 2*idx.second : -2*idx.second-1);
    int C = (int)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((idx.first < 0 && idx.second < 0) || (idx.first >= 0 && idx.second >= 0) ? C : -C - 1);
}

class room {
    public:
        room(const IDX& idx, int dist = 0) {
            this->idx = idx;
            this->_dist = dist;
            parent = nullptr;
            north_ptr = nullptr;
            south_ptr = nullptr;
            east_ptr = nullptr;
            west_ptr = nullptr;
        }
        const IDX& get_idx() const {
            return this->idx;
        }
        int dist() const {
            return this->_dist;
        }
        room* north() const {
            return this->north_ptr;
        }
        void set_north(room* rhs) {
            this->north_ptr = rhs;
        }
        room* south() const {
            return this->south_ptr;
        }
        void set_south(room* rhs) {
            this->south_ptr = rhs;
        }
        room* east() const {
            return this->east_ptr;
        }
        void set_east(room* rhs) {
            this->east_ptr = rhs;
        }
        room* west() const {
            return this->west_ptr;
        }
        void set_west(room* rhs) {
            this->west_ptr = rhs;
        }
        room* parent() const {
            return this->parent_ptr;
        }
        void set_parent(room* rhs) {
            this->parent_ptr = rhs;
        }
    private:
        IDX idx;
        int _dist;
        room* parent_ptr;
        room* north_ptr;
        room* south_ptr;
        room* east_ptr;
        room* west_ptr;
};

typedef std::unordered_map<int,room> rmap;


int build_room_tree(room* origin [[maybe_unused]], rmap& room_list [[maybe_unused]], std::string& regex_line [[maybe_unused]], int regex_idx [[maybe_unused]]) {
}

int get_max_path(room* origin [[maybe_unused]]) {
    int furthest = 0;
    room* dirs[4];
    dirs[0] = origin->north();
    dirs[1] = origin->south();
    dirs[2] = origin->east();
    dirs[3] = origin->west();
    for(int i = 0; i < 4; ++i) {
        if(dirs[i] != nullptr) {
            int dist = get_max_path(dirs[i])+1;
            if(dist > furthest) {
                furthest = dist;
            }
        }
    }
    return furthest;
}

void destroy_tree(room* root) {
    room* dirs[4];
    dirs[0] = origin->north();
    dirs[1] = origin->south();
    dirs[2] = origin->east();
    dirs[3] = origin->west();
    // Delete children
    for(int i = 0; i < 4; ++i) {
        if(dirs[i] != nullptr) {
            destroy_tree(dirs[i]);
        }
    }
    // Delete self.
    delete root;
}

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

    // Initialize 'hash' map of room pointers
    rmap room_list;

    // Create the root room.
    room* root = new room(IDX(0,0));

    // Build the room tree
    build_room_tree(root, room_list, regex_line, 0);

    // Find the longest path
    std::cout << "Furthest room is: " << get_max_path(root) << std::endl;

    // Destroy the tree
    destroy_tree(root);

	return 0;
}
