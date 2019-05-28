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
        bot cross(const bot& rhs) {
            return bot(this->y*rhs.z-this->z*rhs.y,
                       this->z*rhs.x-this->x*rhs.z,
                       this->x*rhs.y-this->y*rhs.x, 0);
        }
        bot dot(const bot& rhs) {
            return this->x*rhs.x+this->y*rhs.y+this->z*rhs.z;
        }
        double mag() {
            return std::sqrt(this->x*this->x+this->y*this->y+this->z*this->z);
        }
        bool operator==(const bot& rhs) const {
            if((this->x != rhs.x)||(this->y != rhs.y)||
               (this->z != rhs.z)||(this->r != rhs.r)) {
                return false;
            } else
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
        bot operator*(double a) const {
            return bot(this->x*a, this->y*a, this->z*a, this->r*a);
        }
        friend std::ostream& operator<<(std::ostream& out, const bot& b);
        bot::type x;
        bot::type y;
        bot::type z;
        bot::type r;
        bot::type num_int;
};

std::unordered_map<int,bot> dirs = {
    // Cardinals
    {0, bot(1,0,0,0)},
    {1, bot(-1,0,0,0)},
    {2, bot(0,1,0,0)},
    {3, bot(0,-1,0,0)},
    {4, bot(0,0,1,0)},
    {5, bot(0,0,-1,0)},
    // 2D combinations
    {6, bot(1,1,0,0)},
    {7, bot(-1,-1,0,0)},
    {8, bot(0,1,1,0)},
    {9, bot(0,-1,-1,0)},
    {10, bot(1,0,1,0)},
    {11, bot(-1,0,-1,0)},
    // 3d combinations
    {12, bot(1,1,1,0)},
    {13, bot(-1,-1,-1,0)},
    {14, bot(1,1,-1,0)},
    {15, bot(-1,-1,1,0)},
    {16, bot(1,-1,-1,0)},
    {17, bot(-1,1,1,0)},
    {18, bot(1,-1,1,0)},
    {19, bot(-1,1,-1,0)},
};


std::unordered_map<int,bot> face_units = {
    {0, bot(1,1,1,0)},
    {1, bot(-1,-1,-1,0)},
    {2, bot(-1,1,1,0)},
    {3, bot(1,-1,-1,0)},
    {4, bot(-1,-1,1,0)},
    {5, bot(1,1,-1,0)},
    {6, bot(1,-1,1,0)},
    {7, bot(-1,1,-1,0)},
}

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

    std::vector<bot> corners;

    // Find corners contained by all bot ranges.
    for(auto it = bots.begin(); it != bots.end(); ++it) {
        bot& b = *it;
        corners.push_back(bot(b.x+b.r,b.y,b.z,0));
        corners.push_back(bot(b.x-b.r,b.y,b.z,0));
        corners.push_back(bot(b.x,b.y+b.r,b.z,0));
        corners.push_back(bot(b.x,b.y-b.r,b.z,0));
        corners.push_back(bot(b.x,b.y,b.z+b.r,0));
        corners.push_back(bot(b.x,b.y,b.z-b.r,0));
    }

    std::unordered_map<int,map_val<bot>> cross_units;

    // For each main intersection
    for(auto it_1 = bots.cbegin(); it_1 != bots.cend()-1; ++it_1) {
        for(auto it_2 = it_1+1; it_2 != bots.cend(); ++it_2) {
            if(it_1->dist(*it_2) <= (it_1->r+it_2->r)) {
                // An intersection is detected!
                
                // For each pair of face normals,
                // Find intersection line if it exists.
                for(int fi_1 = 0; fi_1 < 8; ++fi_1) {
                    for(int fi_2=0; fi_2 < 8; ++fi_2) {
                        // Skip same or parallel face.
                        if((fi_1/2) == (fi_2/2)) {
                            continue;
                        }
                        // Fetch or calculate cross product of units.
                        const bot& norm_1 = face_units[fi_1];
                        bot::type cross_hash = pair_hash_l(fi_1, fi_2);
                        map_val<bot>& line_unit_mapval = cross_units[cross_hash];
                        if(!line_unit_mapval.set) {
                            line_unit_mapval = norm_1.cross(norm_2);
                            // Reduce all elements to 1s.
                            line_unit_mapval.value.x /= 2;
                            line_unit_mapval.value.y /= 2;
                            line_unit_mapval.value.z /= 2;
                        }
                        return false;
                        bot& line_unit = line_unit_mapval.value;
                        // Calculate z=0 point on the line.
                        bot::type d1 = it_1->r;
                        if(norm_1.x == 1) {
                            d1 += it_1->x;
                        } else {
                            d1 -= it_1->x;
                        }
                        if(norm_1.y == 1) {
                            d1 += it_1->y;
                        } else {
                            d1 -= it_1->y;
                        }
                        if(norm_1.z == 1) {
                            d1 += it_1->z;
                        } else {
                            d1 -= it_1->z;
                        }
                        bot::type d2 = it_2->r;
                        if(norm_2.x == 1) {
                            d2 += it_2->x;
                        } else {
                            d2 -= it_2->x;
                        }
                        if(norm_2.y == 1) {
                            d2 += it_2->y;
                        } else {
                            d2 -= it_2->y;
                        }
                        if(norm_2.z == 1) {
                            d2 += it_2->z;
                        } else {
                            d2 -= it_2->z;
                        }

                        bot::type det = norm_1.x*norm_2.y-norm_1.y*norm_2.x;

                        bot line_point((d1*norm_2.y+d2*norm_1.y)/det,-(d1*norm_2.x+d2*norm_1.x),0,0);
                        // Intersect line with planes from first sphere
                        for(int fi = 0; fi < 8; ++fi) {
                            // Skip planes we're intersecting
                            if(fi == fi_1) {
                                continue;
                            }
                            bot& unit = face_units[fi];
                            bot::type d = it_1->r;
                            if(unit.x == 1) {
                                d += it_1->x;
                            } else {
                                d -= it_1->x;
                            }
                            if(unit.y == 1) {
                                d += it_1->y;
                            } else {
                                d -= it_1->y;
                            }
                            if(unit.z == 1) {
                                d += it_1->z;
                            } else {
                                d -= it_1->z;
                            }
                            bot::type s = (d-unit.x*line_point.x-unit.y*line_point.y-unit.z*line_point.z)/(unit.x+unit.y+unit.z);
                            bot point(line_point.x+s*unit.x,line_point.y+s*unit.y,line_point.z+s*unit.z, 0);
                            std::cout << "intersection point: " << point << std::endl;
                            if(it_1->dist(point) <= it_1->r) {
                                std::cout << "Its in the circle!" << std::endl;
                            }
                        }
                    }
                }
                std::cout << "Exit early!!" << std::endl;
                return 0;
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

    int max_int = -1;
    std::vector<bot> max_ps;
    std::for_each(corners.cbegin(), corners.cend(), [&](const bot& b) {
        int nint = num_intersections(b);
        if(nint > max_int) {
            max_int = nint;
            max_ps.clear();
            max_ps.push_back(b);
        } else if (nint == max_int) {
            max_ps.push_back(b);
        }
    });

    std::cout << "Max intersections are: " << max_int << std::endl;
    std::cout << "There are " << max_ps.size() << " corners with this many intersections." << std::endl;

    bot destination(0,0,0,0);
    std::sort(max_ps.begin(), max_ps.end(), [&](const bot& a, const bot& b) {
        return (a.dist(destination) < b.dist(destination));
    });

    bot max_p = max_ps[0];

    bot::type min_dist = max_p.dist(destination);
    bot current_position = max_p;
    bot::type count = 0;
    while(true) {
        std::vector<bot> candidates;
        //std::cout << "considering neighbors" << std::endl;
        for(int d = 0; d < 20; ++d) {
            // We now look at the neighbors.
            bot neighbor = current_position+dirs[d];
            // Check how many intersections
            int intersections = num_intersections(neighbor);
            //std::cout << neighbor << " (" << intersections << ")" << std::endl;
            if(intersections == max_int) {
                // We're still in the intersection region!
                bot::type dist = neighbor.dist(destination);
                if(dist < min_dist) {
                    candidates.push_back(neighbor);
                }
            } else if(intersections > max_int) {
                std::cerr << "There is a region with more intersections! (" << intersections << ")" << std::endl;
                throw;
            }
        }
        if(candidates.size() == 0) {
            // no candidates..
            break;
        }
        std::sort(candidates.begin(), candidates.end(), [&](const bot& a, const bot& b) {
            return (a.dist(destination) < b.dist(destination));
        });
        current_position = candidates[0];
        min_dist = current_position.dist(destination);
        count += 1;
        if(count%10000 == 0) {
            std::cout << "min dist update: " << min_dist << std::endl;
        }
    }

    std::cout << "The closest point in the high signal region is " << min_dist << " units away!" << std::endl;

	return 0;
}
