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

std::unordered_map<long,map_val<int>> emap;

long target_x = 0;
long target_y = 0;
long extra_x = 0;
long extra_y = 0;

long erosion_level(long x, long y) {
    map_val<int>& val = emap[pair_hash_l(x,y)];
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
            std::pair<Value,T> answer = the_queue.back();
            the_queue.pop_back();
            return answer.second;
        }
        template<class functor>
        void for_each(functor f) {
            for(auto it = the_queue.begin(); it != the_queue.end(); ++it) {
                f(*it);
            }
        }
        void sort() {
            std::sort(the_queue.begin(), the_queue.end(), [](auto a, auto b) {
                return (a.first > b.first);
            });
        }
    private:
        std::vector<std::pair<Value,T>> the_queue;
};

namespace tools {
    static const long neither = 0;
    static const long torch = 1;
    static const long gear = 2;
    static const long num = 3;
}

namespace terrain {
    static const long rocky = 0;
    static const long wet = 1;
    static const long narrow = 2;
}

namespace dirs {
    static const int North = 0;
    static const int South = 1;
    static const int East = 2;
    static const int West = 3;
    static const int num = 4;
}

bool is_tool_appropriate(const long terr, const long tool) {
    if(terr == terrain::rocky) {
        if ((tool == tools::torch)||(tool == tools::gear)) {
            return true;
        } else {
            return false;
        }
    } else if (terr == terrain::wet) {
        if ((tool == tools::gear)||(tool == tools::neither)) {
            return true;
        } else {
            return false;
        }
    } else {
        if ((tool == tools::torch)||(tool == tools::neither)) {
            return true;
        } else {
            return false;
        }
    }
}

class node {
    public:
        node(long x, long y, long tool) {
            this->x = x;
            this->y = y;
            this->tool = tool;
        }
        node(const node& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->tool = rhs.tool;
        }
        node& operator=(const node& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            this->tool = rhs.tool;
            return *this;
        }
        long x;
        long y;
        long tool;
        long dist_estimate(const node& rhs) const {
            long est = std::abs(this->x-rhs.x)+std::abs(this->y-rhs.y);
            if(rhs.tool != this->tool) {
                est += 7;
            }
            return est;
        }
        long hash() const {
            return pair_hash_l(pair_hash_l(x,y),tool);
        }
};

void move_in_direction(long& x, long& y, int d) {
    if (d == dirs::North) {
        y -= 1;
    } else if (d == dirs::South) {
        y += 1;
    } else if (d == dirs::East) {
        x += 1;
    } else if (d == dirs::West) {
        x -= 1;
    }
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    bool Map = false;
	ArgParse::ArgParser Parser("Task 44");
    int max = -1;
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-m/--map", "Print map", &Map);
    Parser.AddArgument("-mc/--max-counter", "Set the max counter", &max);

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
    node target(target_x, target_y, tools::torch);
    long target_hash = target.hash();

    node start(0,0,tools::torch);
    queue.insert(start.dist_estimate(target),start);

    std::unordered_map<long,map_val<int>> min_dists;
    min_dists[start.hash()] = 0;

    int counter = 0;
    while((queue.size() != 0)&&(!min_dists[target_hash].set)) {
        if(verbose) {
            std::cout << "Queue diagnostic" << std::endl;
            queue.for_each([](auto& a) {
                std::cout << a.second.x << "," << a.second.y << "," << a.second.tool << ": " << a.first << std::endl;
            });
        }
        // Pop a node off the queue
        node current_node = queue.pop();
        // Move directly to neighbors.
        for(int d = 0; d < dirs::num; ++d) {
            long nx = current_node.x;
            long ny = current_node.y;
            // Move x,y position
            move_in_direction(nx, ny, d);
            // Test that we're in the allowed region.
            if((nx < 0)||(ny < 0)) {
                // Skip if not.
                continue;
            }
            // check that our current tool is appropriate
            if(is_tool_appropriate(erosion_level(nx, ny)%3, current_node.tool)) {
                // Create neighbor node.
                node neighbor(nx,ny,current_node.tool);
                long dist = min_dists[current_node.hash()].value+1;
                if (!min_dists[neighbor.hash()].set) {
                    // Haven't encountered this node yet!
                    // one minute to traverse.
                    min_dists[neighbor.hash()] = dist;
                    // Add the new neighbor in the queue
                    queue.insert(min_dists[neighbor.hash()].value+neighbor.dist_estimate(target), neighbor);
                } else {
                    if(dist < min_dists[neighbor.hash()].value) {
                        min_dists[neighbor.hash()] = dist;
                    }
                }
            }
        }
        // Change tool
        for(int tool = 0; tool < tools::num; ++tool) {
            if(tool != current_node.tool) {
                if(is_tool_appropriate(erosion_level(current_node.x,current_node.y)%3, tool)) {
                    node neighbor(current_node.x, current_node.y, tool);
                    long dist = min_dists[current_node.hash()].value+7;
                    if(!min_dists[neighbor.hash()].set) {
                        // Haven't encountered this node yet!
                        // seven minutes to traverse.
                        min_dists[neighbor.hash()] = dist;
                        // Add the new neighbor to the queue
                        queue.insert(min_dists[neighbor.hash()].value+neighbor.dist_estimate(target), neighbor);
                    } else {
                        if(dist < min_dists[neighbor.hash()].value) {
                            min_dists[neighbor.hash()] = dist;
                        }
                    }
                }
            }
        }
        // Update queue values
        queue.for_each([&min_dists,&target](auto& a) {
            a.first = min_dists[a.second.hash()].value+a.second.dist_estimate(target);
        });
        // Sort queue
        queue.sort();
        // Update weights and sort queue.
        if ((max != -1)&&(counter > max)) {
            break;
        }
        counter += 1;
    }

    if(min_dists[target_hash].set) {
        std::cout << "fastest_route: " << min_dists[target_hash].value << std::endl;
    } else {
        std::cout << "target not reached." << std::endl;
    }

	return 0;
}
