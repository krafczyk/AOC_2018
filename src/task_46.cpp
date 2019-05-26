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
    {1, bot(-1,0,0,0)},
    {2, bot(0,1,0,0)},
    {3, bot(0,-1,0,0)},
    {4, bot(0,0,1,0)},
    {5, bot(0,0,-1,0)}
};

const int Right = 0;
const int Left = 1;
const int Forward = 2;
const int Backward = 3;
const int Up = 4;
const int Down = 5;

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

    std::vector<bot> intersections;

    for(auto it_1 = bots.begin(); it_1 != bots.end()-1; ++it_1) {
        // Compare with bots.
        for(auto it_2 = it_1+1; it_2 != bots.end(); ++it_2) {
            // Detect an intersection
            if(it_1->dist(*it_2) <= it_1->r + it_2->r) {
                bot::type dx = it_1->x-it_2->x;
                bot::type dy = it_1->y-it_2->y;
                bot::type dz = it_1->z-it_2->z;
                bot::type r_sum = it_1->r + it_2->r;
                double fraction = ((double)it_1->r)/((double)r_sum);
                bot::type x = it_1->x+((bot::type)(fraction*dx));
                bot::type y = it_1->x+((bot::type)(fraction*dy));
                bot::type z = it_1->x+((bot::type)(fraction*dz));
                bot intersection(x, y, z, 0);
                mid_points.push_back(mid_point);
            }
        }
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
    bot current_position = median_point;
    bot::type count = 0;
    while(true) {
        bot neighbor_candidate = current_position;
        for(int d = 0; d < 6; ++d) {
            // We now look at the neighbors.
            bot neighbor = current_position+dirs[d];
            // Check how many intersections
            int intersections = num_intersections(neighbor);
            if(intersections == num_intersections_max) {
                // We're still in the intersection region!
                bot::type dist = neighbor.dist(destination);
                if(dist < min_dist) {
                    neighbor_candidate = neighbor;
                }
            } else if(intersections > num_intersections_max) {
                std::cerr << "There is a region with more intersections! (" << intersections << ")" << std::endl;
                throw;
            }
        }
        bot::type dist = neighbor_candidate.dist(destination);
        if(dist >= min_dist) {
            // We've reached the end of the line!
            break;
        } else {
            current_position = neighbor_candidate;
            min_dist = dist;
        }
        count += 1;
        if(count%10000 == 0) {
            std::cout << "min dist update: " << min_dist << std::endl;
        }
    }

    std::cout << "The closest point in the high signal region is " << min_dist << " units away!" << std::endl;

	return 0;
}
