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

inline int hash(const IDX& idx) {
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

std::unordered_map<int, char> map_dir = {
    {North, 'N'},
    {South, 'S'},
    {East, 'E'},
    {West, 'W'}
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
            for(int i = 0; i < 5; ++i) {
                this->ptr_storage[i] = nullptr;
            }
        }
        const IDX& get_idx() const {
            return this->idx;
        }
        int dist() const {
            return this->_dist;
        }
        void set_dist(int dist) {
            this->_dist = dist;
        }
        room* neighbor(int direction) const {
            return this->ptr_storage[direction];
        }
        void set_neighbor(room* rhs, int direction) {
            this->ptr_storage[direction] = rhs;
        }
        room* parent() const {
            return this->ptr_storage[4];
        }
        void set_parent(room* rhs) {
            this->ptr_storage[4] = rhs;
        }
        void show(std::ostream& out) {
            out << idx.first << "," << idx.second << ":" << _dist;
        }
    private:
        IDX idx;
        int _dist;
        room* ptr_storage[5];
};

typedef std::unordered_map<int,room*> rmap;


void build_room_tree(room* origin, rmap& room_list, std::string& regex_line, int regex_idx) {
    printf("build_room_tree called with origin %i,%i\n", origin->get_idx().first, origin->get_idx().second);
    IDX original_idx = origin->get_idx();

    auto advance_index = [](const std::string& the_string, int& idx) {
        int level = 0;
        while((size_t)idx < the_string.size()) {
            if(level == 0) {
                if((the_string[idx] == '|')||(the_string[idx] == ')')) {
                    break;
                }
            }
            if(the_string[idx] == '(') {
                level += 1;
            }
            if(the_string[idx] == ')') {
                level -= 1;
            }
            idx += 1;
        }
    };


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
            // Manage searching of new branches
            do {
                regex_idx += 1;
                build_room_tree(origin, room_list, regex_line, regex_idx);
                advance_index(regex_line, regex_idx);
            } while(regex_line[regex_idx] != ')');

            // Started all sub branches from this point. can exit!
            break;
        }
        if(regex_line[regex_idx] == '|') {
            // Reached end of current branch section. Should skip until ')' at the current level.
            while(regex_line[regex_idx] != ')') {
                regex_idx += 1;
                advance_index(regex_line, regex_idx);
            }
        }
        if(regex_line[regex_idx] == ')') {
            // We should skip this character because we're at the end of a branch section.
            regex_idx += 1;
            continue;
        }
        if((regex_line[regex_idx] == 'N')||
           (regex_line[regex_idx] == 'S')||
           (regex_line[regex_idx] == 'E')||
           (regex_line[regex_idx] == 'W')) {
            int direction = dir_map[regex_line[regex_idx]];
            IDX new_room_idx = move_in_direction(origin->get_idx(), direction);
            int new_room_idx_hash = hash(new_room_idx);
            if(room_list[new_room_idx_hash] == nullptr) {
                // Need to add a room!
                room* new_room = new room(new_room_idx, origin->dist()+1);
                origin->set_neighbor(new_room, direction);
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
                    for(int i = 0; i < NumDirs; ++i ) {
                        if(parent->neighbor(i) == the_room) {
                            parent->set_neighbor(nullptr, i);
                        }
                    }
                    // Set pointers
                    the_room->set_parent(origin);
                    origin->set_neighbor(the_room, direction);
                } else {
                    // This is not a strictly better path. Just advance to this room.
                    origin = the_room;
                }
                // Advance the character index
                regex_idx += 1;
            }
        } else {
            std::cerr << "Main loop error! don't recognize character!" << std::endl;
            exit(-1);
        }
    }
    printf("End of build call originating at %i,%i\n", original_idx.first, original_idx.second);
}

int get_max_path(room* origin [[maybe_unused]]) {
    int furthest = 0;
    for(int i = 0; i < NumDirs; ++i) {
        if(origin->neighbor(i) != nullptr) {
            int dist = get_max_path(origin->neighbor(i))+1;
            if(dist > furthest) {
                furthest = dist;
            }
        }
    }
    return furthest;
}

void destroy_tree(room* root) {
    // Delete children
    for(int i = 0; i < NumDirs; ++i) {
        if(root->neighbor(i) != nullptr) {
            destroy_tree(root->neighbor(i));
        }
    }
    // Delete self.
    delete root;
}

void show_tree(room* root) {
    root->show(std::cout);
    std::cout << " ";
    for(int i = 0; i < NumDirs; ++i) {
        if(root->neighbor(i) != nullptr) {
            std::cout << map_dir[i] << ":( ";
            show_tree(root->neighbor(i));
            std::cout << " ) ";
        }
    }
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

    show_tree(root);
    std::cout << std::endl;

    // Find the longest path
    std::cout << "Furthest room is: " << get_max_path(root) << std::endl;

    // Destroy the tree
    destroy_tree(root);

	return 0;
}
