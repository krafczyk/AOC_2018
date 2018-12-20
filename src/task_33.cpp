#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <iomanip>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
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

char get_tile(const point& p, const std::vector<Range>& Clay, const std::vector<Range>& WaterPassed, const std::vector<Range>& Water) {
    if(p == point(500,0)) {
        return '+';
    }
    for(auto it = Clay.cbegin(); it != Clay.cend(); ++it) {
        if(it->contains(p)) {
            return '#';
        }
    }
    for(auto it = WaterPassed.cbegin(); it != WaterPassed.cend(); ++it) {
        if(it->contains(p)) {
            return '|';
        }
    }
    for(auto it = Water.cbegin(); it != Water.cend(); ++it) {
        if(it->contains(p)) {
            return '~';
        }
    }
    return '.';
}

void print_state(std::ostream& out, point::p_idx x_min, point::p_idx x_max, point::p_idx y_max, const std::vector<Range>& Clay, const std::vector<Range>& WaterPassed, const std::vector<Range>& Water) {
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
    ArgParse::ArgParser Parser("Task 33");
    Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-o/--output-file", "A file to put a copy of the output", &output_filepath, ArgParse::Argument::Optional, &output_filepath_passed);

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

    std::vector<Range> WaterPassed;
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
    WaterPassed.push_back(Range(point(500,1),point(500,1)));

    size_t state [[maybe_unused]] = 0;

    // Not sure what the stopping condition should be.
    bool changed = true;
    //size_t count = 0;
    while(changed) {
        //std::cout << "New Loop" << std::endl;
        changed = false;
        for(auto it = WaterPassed.begin(); it != WaterPassed.end(); ++it) {
            //std::cout << "Considering range: " << it->get_str() << std::endl;
            //std::cout << "All WaterPassed Ranges:" << std::endl;
            //ConstForEach(WaterPassed, [](const Range& rng) {
            //        std::cout << rng.get_str() << std::endl;
            //});
            //std::cout << "All Water Ranges:" << std::endl;
            //ConstForEach(Water, [](const Range& rng) {
            //        std::cout << rng.get_str() << std::endl;
            //});
            // Find and extend vertical groups
            if(it->is_vertical()) {
                // Check bottom
                char tile = get_tile(point(it->get_min().get_x(), it->get_max().get_y()+1),
                                     Clay, WaterPassed, Water);
                if(tile == '.') {
                    // Extend down until you hit another tile.
                    point::p_idx temp_y = it->get_max().get_y()+1;
                    //std::cout << "y_max: " << y_max << std::endl;
                    char down_tile = get_tile(point(it->get_min().get_x(), temp_y+1), Clay, WaterPassed, Water);
                    while((down_tile == '.')&&(temp_y <= y_max)) {
                        ++temp_y;
                        down_tile = get_tile(point(it->get_min().get_x(), temp_y+1), Clay, WaterPassed, Water);
                    }
                    if(temp_y > y_max) {
                        temp_y = y_max;
                    }
                    down_tile = get_tile(point(it->get_min().get_x(), temp_y+1), Clay, WaterPassed, Water);
                    //std::cout << "temp_y: " << temp_y << std::endl;
                    //std::cout << "it->get_max().get_y(): " << it->get_max().get_y() << std::endl;
                    if(temp_y != it->get_max().get_y()) {
                        //std::cout << "diff: " << temp_y-it->get_max().get_y() << std::endl;
                        it->mod_down(temp_y-it->get_max().get_y());
                        //std::cout << "it->get_max().get_y(): " << it->get_max().get_y() << std::endl;
                        //std::cout << "change 1" << std::endl;
                        changed = true;
                    }
                }
                // Load new tile after an extend.
                if(changed) {
                    tile = get_tile(point(it->get_min().get_x(), it->get_max().get_y()+1),
                                          Clay, WaterPassed, Water);
                }
                if((tile == '#')||(tile == '~')) {
                    // We have water.
                    point::p_idx x_left = it->get_min().get_x();
                    point::p_idx x_right = it->get_max().get_x();
                    point::p_idx y = it->get_max().get_y();
                    // search left
                    bool left_wall = false;
                    char left_tile = get_tile(point(x_left-1,y), Clay, WaterPassed, Water);
                    char down_tile = get_tile(point(x_left,y+1), Clay, WaterPassed, Water);
                    while((left_tile == '.')&&((down_tile == '#')||(down_tile == '~'))) {
                        //std::cout << "Searching left" << std::endl;
                        --x_left;
                        left_tile = get_tile(point(x_left-1,y), Clay, WaterPassed, Water);
                        down_tile = get_tile(point(x_left,y+1), Clay, WaterPassed, Water);
                    }
                    if(left_tile == '#') {
                        left_wall = true;
                    }
                    // search right
                    bool right_wall = false;
                    char right_tile = get_tile(point(x_right+1,y), Clay, WaterPassed, Water);
                    down_tile = get_tile(point(x_right,y+1), Clay, WaterPassed, Water);
                    while((right_tile == '.')&&((down_tile == '#')||(down_tile == '~'))) {
                        //std::cout << "Searching right" << std::endl;
                        ++x_right;
                        right_tile = get_tile(point(x_right+1,y), Clay, WaterPassed, Water);
                        down_tile = get_tile(point(x_right,y+1), Clay, WaterPassed, Water);
                    }
                    if(right_tile == '#') {
                        right_wall = true;
                    }
                    it->mod_down(-1);
                    //std::cout << "change 2" << std::endl;
                    changed = true;
                    if(left_wall&&right_wall) {
                        Range rng(point(x_left,y),point(x_right,y));
                        // Find whether there's a compatible water range already
                        bool found_compatible = false;
                        for(auto r_it = Water.begin(); r_it != Water.end(); ++r_it) {
                            if((r_it->get_min().get_x() == x_left)&&
                               (r_it->get_max().get_x() == x_right)&&
                               (r_it->get_min().get_y() == y+1)) {
                                found_compatible = true;
                                //std::cout << "Stretched one up!" << std::endl;
                                r_it->mod_up(1);
                                break;
                            }
                        }
                        if(!found_compatible) {
                            //std::cout << "Add new water range!" << std::endl;
                            Water.push_back(Range(point(x_left,y),point(x_right,y)));
                        }
                    } else {
                        //std::cout << "Adding more water passing" << std::endl;
                        // Check for 'compatible' water passing.
                        bool found_compatible = false;
                        for(auto r_it = WaterPassed.begin(); r_it != WaterPassed.end(); ++r_it) {
                            if(r_it->is_horizontal()) {
                                if((r_it->get_min().get_y() == y)&&
                                   (r_it->get_min().get_x() <= x_right)&&
                                   (r_it->get_max().get_x() >= x_left)) {
                                    found_compatible = true;
                                }
                            }
                        }
                        if(found_compatible) {
                            throw std::runtime_error("This shouldn't happen!");
                        }
                        WaterPassed.push_back(Range(point(x_left,y),point(x_right,y)));
                        if(get_tile(point(x_left,y+1), Clay, WaterPassed, Water) != '~') {
                            WaterPassed.push_back(Range(point(x_left,y+1),point(x_left,y+1)));
                        }
                        if(get_tile(point(x_right,y+1), Clay, WaterPassed, Water) != '~') {
                            WaterPassed.push_back(Range(point(x_right,y+1),point(x_right,y+1)));
                        }
                        //std::cout << "Special State" << std::endl;
                        //print_state(x_min, x_max, y_max, Clay, WaterPassed, Water);
                        break;
                    }
                }
            }
        }
        //std::cout << "New State" << std::endl;
        //print_state(x_min, x_max, y_max, Clay, WaterPassed, Water);

        //++count;
        //if(count > 12) {
        //    throw std::runtime_error("Stop early");
        //}
    }

    //std::cout << "Checking overlap" << std::endl;
    // Check that there is no overlap
    for(auto it = WaterPassed.cbegin(); it != WaterPassed.cend(); ++it) {
        for(auto it_2 = Water.cbegin(); it_2 != Water.cend(); ++it_2) {
            if(it_2->interferes(*it)) {
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
    ConstForEach(WaterPassed, [&](const Range& rng) {
        Water_total += rng.size();
    });
    std::cout << "There are a total of " << Water_total << " tiles the water can reach." << std::endl;
    return 0;
}
