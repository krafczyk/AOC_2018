#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <iomanip>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <utility>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

class point {
    public:
        typedef int p_idx;
        point() {
            this->x = 0;
            this->y = 0;
        }
        point(p_idx x, p_idx y) {
            this->x = x;
            this->y = y;
        }
        point(const point& rhs) = default;
        point& operator=(const point& rhs) = default;
        p_idx get_x() const {
            return this->x;
        }
        p_idx get_y() const {
            return this->y;
        }
        void set_x(p_idx in) {
            this->x = in;
        }
        void set_y(p_idx in ) {
            this->y = in;
        }
        bool operator==(const point& in) const {
            if((this->x == in.x)&&(this->y == in.y)) {
                return true;
            } else {
                return false;
            }
        }
        point operator+(const point& in) const {
            point answer = *this;
            answer.x = in.x;
            answer.y = in.y;
            return answer;
        }
        bool operator<(const point& rhs) const {
            if(this->y < rhs.y) {
                return true;
            } else if (this->y == rhs.y) {
                if(this->x < rhs.x) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        std::string get_str() const {
            std::stringstream ss;
            ss << this->x << "," << this->y;
            return ss.str();
        }
    private:
        p_idx x;
        p_idx y;
        
};

class Range {
    public:
        Range(const point& P1, const point& P2) {
            this->min = point(std::min(P1.get_x(),P2.get_x()),
                      std::min(P1.get_y(),P2.get_y()));
            this->max = point(std::max(P1.get_x(),P2.get_x()),
                      std::max(P1.get_y(),P2.get_y()));
        }
        static bool inline in_extent(point::p_idx low, point::p_idx high, point::p_idx e) {
            if((e <= high)&&(e >= low)) {
                return true;
            } else {
                return false;
            }
        }
        bool contains(const point& p) const {
            if(in_extent(min.get_x(),max.get_x(),p.get_x())&&
               in_extent(min.get_y(),max.get_y(),p.get_y())) {
                return true;
            } else {
                return false;
            }
        }
        bool interferes(const Range& rng) const {
            if((this->max.get_x() >= rng.min.get_x())||
               (rng.max.get_x() >= this->min.get_x())||
               (this->max.get_y() >= rng.min.get_y())||
               (rng.max.get_y() >= this->min.get_y())) {
                return false;
            } else {
                return true;
            }
        }
        bool is_vertical() const {
            if((this->max.get_y()-this->min.get_y()+1) >= (this->max.get_x()-this->min.get_x()+1)) {
                return true;
            } else {
                return false;
            }
        }
        bool is_horizontal() const {
            if((this->max.get_x()-this->min.get_x()+1) >= (this->max.get_y()-this->min.get_y()+1)) {
                return true;
            } else {
                return false;
            }
        }
        const point& get_min() const {
            return this->min;
        }
        const point& get_max() const {
            return this->max;
        }
        size_t size() const {
            return (this->max.get_x()-this->min.get_x()+1)*(this->max.get_y()-this->min.get_y()+1);
        }
        void mod_down(point::p_idx mod) {
            this->max.set_y(this->max.get_y()+mod);
        }
        void mod_up(point::p_idx mod) {
            this->min.set_y(this->min.get_y()-mod);
        }
        void mod_left(point::p_idx mod) {
            this->min.set_x(this->min.get_x()-mod);
        }
        void mod_right(point::p_idx mod) {
            this->max.set_x(this->max.get_x()+mod);
        }
        std::string get_str() const {
            std::stringstream ss;
            ss << "(" << this->min.get_str() << ";" << this->max.get_str() << ")";
            return ss.str();
        }
    private:
        point min;
        point max;
};

char get_tile(const point& p, const std::vector<Range>& Clay, const std::set<point>& WaterPassed, const std::vector<Range>& Water) {
    if(p == point(500,0)) {
        return '+';
    }
    for(auto it = Clay.cbegin(); it != Clay.cend(); ++it) {
        if(it->contains(p)) {
            return '#';
        }
    }
    if(hasElement(WaterPassed, p)) {
        return '|';
    }
    for(auto it = Water.cbegin(); it != Water.cend(); ++it) {
        if(it->contains(p)) {
            return '~';
        }
    }
    return '.';
}

void print_state(std::ostream& out, point::p_idx x_min, point::p_idx x_max, point::p_idx y_max, const std::vector<Range>& Clay, const std::set<point>& WaterPassed, const std::vector<Range>& Water) {
    // Determine number of digits
    point::p_idx n_digits = ((point::p_idx) std::log10(y_max))+1;

    // Print the rest of the lines
    for(point::p_idx y = 0; y <= y_max; ++y) {
        out << std::setfill(' ') << std::setw(n_digits) << y << " ";
        for(point::p_idx x = x_min; x <= x_max; ++x) {
            out << get_tile(point(x,y), Clay, WaterPassed, Water);
        }
        out << std::endl;
    }
}

int main(int argc, char** argv) {
    // Parse Arguments
    std::string input_filepath;
    std::string output_filepath;
    bool output_filepath_passed = false;
    bool verbose = false;
    int count_max = 0;
    ArgParse::ArgParser Parser("Task 33");
    Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-o/--output-file", "A file to put a copy of the output", &output_filepath, ArgParse::Argument::Optional, &output_filepath_passed);
    Parser.AddArgument("-c/--count-max", "Set count", &count_max, ArgParse::Argument::Optional);

    if (Parser.ParseArgs(argc, argv) < 0) {
        std::cerr << "Problem parsing arguments!" << std::endl;
        return -1;
    }

    if (Parser.HelpPrinted()) {
        return 0;
    }

    // Open input as stream
    std::ifstream infile(input_filepath);

    std::string line;
    std::regex line_regex("([xy])=(\\d+), ([xy])=(\\d+)\\.\\.(\\d+)", std::regex::ECMAScript);
    std::vector<Range> Clay;
    while(std::getline(infile, line)) {
        std::smatch res;
        if(std::regex_match(line, res, line_regex)) {
            if (res[1] == "x") {
                point::p_idx x = std::atoi(res[2].str().c_str());
                point::p_idx y_min = std::atoi(res[4].str().c_str());
                point::p_idx y_max = std::atoi(res[5].str().c_str());
                Clay.push_back(Range(point(x,y_min),point(x,y_max)));
            } else {
                point::p_idx y = std::atoi(res[2].str().c_str());
                point::p_idx x_min = std::atoi(res[4].str().c_str());
                point::p_idx x_max = std::atoi(res[5].str().c_str());
                Clay.push_back(Range(point(x_min,y),point(x_max,y)));
            }
        }
    }

    std::set<point> WaterPassed;
    std::vector<Range> Water;

    point::p_idx x_min = std::numeric_limits<point::p_idx>::max();
    point::p_idx x_max = std::numeric_limits<point::p_idx>::min();
    point::p_idx y_min = std::numeric_limits<point::p_idx>::max();
    point::p_idx y_max = std::numeric_limits<point::p_idx>::min();
    ConstForEach(Clay, [&](const Range& Rng) {
        if(Rng.get_max().get_x() > x_max) {
            x_max = Rng.get_max().get_x();
        }
        if(Rng.get_min().get_x() < x_min) {
            x_min = Rng.get_min().get_x();
        }
        if(Rng.get_max().get_y() > y_max) {
            y_max = Rng.get_max().get_y();
        }
        if(Rng.get_min().get_y() < y_min) {
            y_min = Rng.get_min().get_y();
        }
    });
    // Widen by one..
    x_min -= 1;
    x_max += 1;
    y_min = 1;

    if(verbose) {
        std::cout << "Initial State" << std::endl;
        print_state(std::cout, x_min, x_max, y_max, Clay, WaterPassed, Water);
    }

    // Start sim

    // Initial range.
    WaterPassed.insert(point(500,1));

    size_t state [[maybe_unused]] = 0;

    // Not sure what the stopping condition should be.
    for(auto it = WaterPassed.begin(); it != WaterPassed.end();) {
        //std::cout << "Start Major Loop" << std::endl;
        char down_tile = get_tile(point(it->get_x(),it->get_y()+1), Clay, WaterPassed, Water);
        // Always extend down if empty.?
        if(down_tile == '.') {
            //std::cout << "Start Major Condition 1" << std::endl;
            bool added = false;
            if(it->get_y()+1 <= y_max) {
                // Only if we're not off the max.
                WaterPassed.insert(it, point(it->get_x(),it->get_y()+1));
                added = true;
                // We don't need to treat the iterator in any special way since we're always adding after.
            }
            ++it;
            //std::cout << "End Major Condition 1" << std::endl;
        } else if((down_tile == '~')||(down_tile == '#')) {
            //std::cout << "Start Major Condition 2" << std::endl;

            // We look left to right to see whether we should
            // Add a water line.
            point::p_idx y = it->get_y();
            point::p_idx x_left = it->get_x();
            point::p_idx x_right = it->get_x();
            bool left_wall = false;
            bool right_wall = false;
            while(true) {
                char left_tile = get_tile(point(x_left-1,y), Clay, WaterPassed, Water);
                char temp_down = get_tile(point(x_left,y+1), Clay, WaterPassed, Water);
                if((temp_down == '~')||(temp_down == '#')) {
                    if(left_tile == '#') {
                        // Found a wall
                        left_wall = true;
                        break;
                    } else {
                        // Move left
                        --x_left;
                    }
                } else {
                    // Otherwise, we have to stop.
                    break;
                }
            }
            while(true) {
                char right_tile = get_tile(point(x_right+1,y), Clay, WaterPassed, Water);
                char temp_down = get_tile(point(x_right,y+1), Clay, WaterPassed, Water);
                if((temp_down == '~')||(temp_down == '#')) {
                    if(right_tile == '#') {
                        // Found a wall
                        right_wall = true;
                        break;
                    } else {
                        // Move right
                        ++x_right;
                    }
                } else {
                    // Otherwise, we have to stop.
                    break;
                }
            }
            // We need to add a line, the question is whether its water or not.
            if(left_wall&&right_wall) {


                // Water
                // Check for compatible ranges we can use to stretch up.
                bool found_compatible = false;
                for(auto r_it = Water.begin(); r_it != Water.end(); ++r_it) {
                    if((r_it->get_min().get_x() == x_left)&&
                       (r_it->get_max().get_x() == x_right)&&
                       (r_it->get_min().get_y() == y+1)) {
                        found_compatible = true;
                        r_it->mod_up(1);
                        // Back it up one row.
                        while(it->get_y() > y-1) {
                            --it;
                        }
                        // // Remove passing elements in this new expanded range.
                        for(auto p_it = WaterPassed.begin(); p_it != WaterPassed.end();) {
                                if(r_it->contains(*p_it)) {
                                    auto distance = std::distance(it, p_it);
                                    if(distance <= 0) {
                                        // Move the iterator in front of the deleted elements.
                                        std::advance(it, distance-1);
                                    }
                                    p_it = WaterPassed.erase(p_it);
                                } else {
                                    ++p_it;
                                }
                        }
                        //std::cout << "Special state 2" << std::endl;
                        //print_state(std::cout, x_min, x_max, y_max, Clay, WaterPassed, Water);
                        break; // break from range loop
                    }
                }
                if(!found_compatible) {
                    // Need to add a new range.
                    Range rng(point(x_left, y),point(x_right,y));
                    Water.push_back(rng);
                    // Remove passing elements in this new expanded range.
                    for(auto p_it = WaterPassed.begin(); p_it != WaterPassed.end();) {
                        if(rng.contains(*p_it)) {
                            auto distance = std::distance(it, p_it);
                            if(distance <= 0) {
                                // Move the iterator in front of the deleted elements.
                                std::advance(it, distance-1);
                            }
                            p_it = WaterPassed.erase(p_it);
                        } else {
                            ++p_it;
                        }
                    }
                }
            } else {
                // Add water passing. May not be a new thing.
                bool added = false;
                //std::cout << "Start, x_left: " << x_left << " x_right: " << x_right << " y: " << y << std::endl;
                for(point::p_idx x_temp = x_left; x_temp <= x_right; ++x_temp) {
                    //std::cout << "miniloop" << std::endl;
                    // Add if it doesn't exist already.
                    point p(x_temp,y);
                    if(!hasElement(WaterPassed, p)) {
                        //std::cout << "Inserting" << std::endl;
                        added = true;
                        WaterPassed.insert(p);
                        // Move it to the line before y.
                        // distance locks up for me.. I don't know why...
                        //auto distance = std::distance(it, temp_it);
                        //std::cout << "distance: " << distance << std::endl;
                        //if(distance <= 0) {
                        //    std::advance(it, distance-1);
                        //}
                    }
                }
                //std::cout << "Special state 3" << std::endl;
                //print_state(std::cout, x_min, x_max, y_max, Clay, WaterPassed, Water);
                if(!added) {
                    //std::cout << "Nothing added." << std::endl;
                    ++it;
                } else {
                    //std::cout << "Something added, moving it." << std::endl;
                    while(it->get_y() >= y) {
                        std::advance(it, -1);
                    }
                    //std::cout << "Finished moving it." << std::endl;
                }
                //std::cout << "End Loop" << std::endl;
            }
            //std::cout << "End Major Condition 2" << std::endl;
        } else {
            //std::cout << "Start Major Condition 3" << std::endl;
            //std::cout << "Advancing" << std::endl;
            ++it;
            //std::cout << "End Advancing" << std::endl;
            //std::cout << "End Major Condition 3" << std::endl;
        }
        //std::cout << "End Major Loop" << std::endl;
    }

    //std::cout << "Checking overlap" << std::endl;
    // Check that there is no overlap
    for(auto it = WaterPassed.cbegin(); it != WaterPassed.cend(); ++it) {
        for(auto it_2 = Water.cbegin(); it_2 != Water.cend(); ++it_2) {
            if(it_2->contains(*it)) {
                throw std::runtime_error("WaterPassed has members in Water!");
            }
        }
    }

    if(verbose) {
        std::cout << "Current State" << std::endl;
        print_state(std::cout, x_min, x_max, y_max, Clay, WaterPassed, Water);
    }

    if(output_filepath_passed) {
        std::ofstream ofile(output_filepath);
        print_state(ofile, x_min, x_max, y_max, Clay, WaterPassed, Water);
    }

    size_t Water_total = 0;
    ConstForEach(Water, [&](const Range& rng) {
        Water_total += rng.size();
    });
    std::cout << "There are a total of " << Water_total+WaterPassed.size() << " tiles the water can reach." << std::endl;
    return 0;
}
