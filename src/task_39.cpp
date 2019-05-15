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

const int North = 0;
const int South = 1;
const int East = 2;
const int West = 3;
const int NumDirs = 4;

std::unordered_map<char,int> dir_map = {
    {'N', North},
    {'S', South},
    {'E', East},
    {'W', West}
};

IDX move_in_direction(IDX idx, int direction) {
    switch (direction) {
        case North: {
            idx.second += 1;
            return idx;
        }
        case South: {
            idx.second -= 1;
            return idx;
        }
        case East: {
            idx.first += 1;
            return idx;
        }
        case West: {
            idx.first -= 1;
            return idx;
        }
        default: {
            return idx;
        }
    }
}

class room {
    public:
        room(const IDX& idx, int dist = 0) {
            this->idx = idx;
            this->_dist = dist;
            parent_ptr = nullptr;
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

typedef std::unordered_map<int,room*> rmap;


void build_room_tree(room* origin [[maybe_unused]], rmap& room_list [[maybe_unused]], std::string& regex_line [[maybe_unused]], int regex_idx [[maybe_unused]]) {
    while(true) {
        // End if at the end of the regex.
        if((size_t) regex_idx >= regex_line.size()) {
            break;
        }
        if(regex_line[regex_idx] == '^') {
            // Skip the beginning
            regex_idx += 1;
            continue;
        }
        if(regex_line[regex_idx] == '$') {
            // Skip the end
            regex_idx += 1;
            continue;
        }
        if(regex_line[regex_idx] == '(') {
            std::set<int> endpoints;
            regex_idx += 1;
            build_room_tree(origin, room_list, regex_line, regex_idx);
        }
        if(regex_line[regex_idx] == '|') {
        }
        if(regex_line[regex_idx] == ')') {
        }
        if(regex_line[regex_idx] == 'N') {
            IDX new_room_idx = origin->get_idx();
            new_room_idx.second += 1;
            int new_room_idx_hash = hash(new_room_idx);
            if(room_list[new_room_idx_hash] == nullptr) {
                // Need to add a room!
                room* new_room = new room(new_room_idx, origin->dist()+1);
                origin->set_north(new_room);
                new_room->set_parent(origin);
                // Advance to the new room!
                origin = new_room;
                // Advance the character index
                regex_idx += 1;
            } else {
                // Room exists already!
                room* the_room = room_list[new_room_idx_hash];
                // Check if we've found a better path!
                if(origin->dist()+1 < the_room->dist()) {
                    // We did find a better path!
                    // We first need to remove the room as a child of the parent.
                    room* parent = the_room->parent();
                    if(parent->north() == the_room) {
                        parent->set_north(nullptr);
                    } else if (parent->south() == the_room) {
                        parent->set_south(nullptr);
                    } else if (parent->east() == the_room) {
                        parent->set_east(nullptr);
                    } else if (parent->west() == the_room) {
                        // This conditional may not be necessary.
                        parent->set_west(nullptr);
                    } else {
                        std::cerr << "This shouldn't happen!" << std::endl;
                        exit(-1);
                    }
                    // Set pointers
                    the_room->set_parent(origin);
                    origin->set_north(the_room);
                } else {
                    // This is not a strictly better path. Just advance to this room.
                    origin = the_room;
                }
                // Advance the character index
                regex_idx += 1;
            }
        }
    }
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
    dirs[0] = root->north();
    dirs[1] = root->south();
    dirs[2] = root->east();
    dirs[3] = root->west();
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
