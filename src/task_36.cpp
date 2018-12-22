#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <ncurses.h>
#include "ArgParseStandalone.h"
#include "utilities.h"

class lumberyard_state {
    public:
        lumberyard_state(int in_size) {
            this->size = in_size;
            lumberyard = new char[size*size];
            this->line = new char[size+1];
            for(int idx = 0; idx < size*size; ++idx) {
                lumberyard[idx] = 0;
            }
        }
        ~lumberyard_state() {
            delete [] lumberyard;
            delete [] line;
        }
        char& assign(int x_idx, int line_idx) {
            return lumberyard[line_idx*size+x_idx];
        }
        char operator()(int x_idx, int line_idx) const {
            return lumberyard[line_idx*size+x_idx];
        }
        std::map<char,int> neighbor_stats(int i, int j) const {
            std::map<char,int> the_map;
            for(int i_diff = -1; i_diff <= 1; ++i_diff) {
                for(int j_diff = -1; j_diff <= 1; ++j_diff) {
                    if((j_diff == 0)&&(i_diff == 0)) {
                        // Skip the middle.
                        continue;
                    }
                    int i_idx = i+i_diff;
                    int j_idx = j+j_diff;
                    if((i_idx >= 0)&&(j_idx >= 0)&&
                       (i_idx < size)&&(j_idx < size)) {
                        the_map[(*this)(i_idx,j_idx)] += 1;
                    }
                }
            }
            return the_map;
        }
        std::map<char,int> all_stats() const {
            std::map<char,int> the_map;
            for(int line_idx = 0; line_idx < size; ++line_idx) {
                for(int x_idx = 0; x_idx < size; ++x_idx) {
                    the_map[(*this)(line_idx,x_idx)] += 1;
                }
            }
            return the_map;
        }
        void print(std::ostream& out) const {
            for(int line_idx = 0; line_idx < size; ++line_idx) {
                for(int x_idx = 0; x_idx < size; ++x_idx) {
                    out << (*this)(line_idx,x_idx);
                }
                out << std::endl;
            }
        }
        void print_ncurses(long minute) const {
            int row,col;
            getmaxyx(stdscr,row,col);
            int line_idx = 0;
            while((line_idx < size)&&(line_idx < row)) {
                int x_idx = 0;
                while((x_idx < size)&&(x_idx < col)) {
                    line[x_idx] = (*this)(line_idx,x_idx);
                    ++x_idx;
                }
                // Add null character.
                line[x_idx+1] = 0;
                mvprintw(line_idx, 0, "%s", this->line);
                ++line_idx;
            }
            // Print minutes
            std::stringstream ss;
            ss << minute;
            mvprintw(0, size+3, "minute: %s", ss.str().c_str());
            refresh();
        }
        int resource_value() const {
            auto the_map = this->all_stats();
            return the_map['|']*the_map['#'];
        }
    private:
        int size;
        char* line;
        char* lumberyard;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    bool super_verbose = false;
    int size = 50;
    long num_minutes = 0;
	ArgParse::ArgParser Parser("Task 36");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-n/--number-minutes", "Number of minutes to simulate at most", &num_minutes);
    Parser.AddArgument("-s/--size", "Specify size", &size, ArgParse::Argument::Optional);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
    Parser.AddArgument("-sv/--super-verbose", "Print Extra Verbose output", &super_verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    // Create Lumberyards
    lumberyard_state* current_lumberyard = new lumberyard_state(size);
    lumberyard_state* next_lumberyard [[maybe_unused]]= new lumberyard_state(size);

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;
    int line_idx = 0;
    while(std::getline(infile, line)) {
        for(int idx=0; idx < (int) line.size(); ++idx) {
            current_lumberyard->assign(line_idx,idx) = line[idx];
        }
        line_idx += 1;
    }

    // Here we initialize ncurses.
    initscr();

    if(verbose) {
        current_lumberyard->print(std::cout);
    }

    long minutes = 0;
    int total_repeats = 3;
    std::vector<int> prev_resource_values;
    std::map<int,int> repeats;
    prev_resource_values.push_back(current_lumberyard->resource_value());
    while(minutes < num_minutes) {
        // Quit here
        int max_repeats = std::numeric_limits<int>::min();
        ConstForEach(repeats, [&](const std::pair<int,int>& el) {
                if(el.second > max_repeats) {
                    max_repeats = el.second;
                }
        });
        if(max_repeats == total_repeats) {
            break;
        }
        // Get next lumberyard
        for(int line_idx = 0; line_idx < size; ++line_idx) {
            for(int x_idx = 0; x_idx < size; ++x_idx) {
                auto neighbor_stats = current_lumberyard->neighbor_stats(line_idx, x_idx);
                char tile = (*current_lumberyard)(line_idx,x_idx);
                if(tile == '.') {
                    if(neighbor_stats['|'] >= 3) {
                        next_lumberyard->assign(line_idx, x_idx) = '|';
                    } else {
                        next_lumberyard->assign(line_idx, x_idx) = '.';
                    }
                } else if (tile == '|') {
                    if(neighbor_stats['#'] >= 3) {
                        next_lumberyard->assign(line_idx, x_idx) = '#';
                    } else {
                        next_lumberyard->assign(line_idx, x_idx) = '|';
                    }
                } else if (tile == '#') {
                    if((neighbor_stats['|'] >= 1)&&(neighbor_stats['#']>=1)) {
                        next_lumberyard->assign(line_idx, x_idx) = '#';
                    } else {
                        next_lumberyard->assign(line_idx, x_idx) = '.';
                    }
                } else {
                    throw std::runtime_error("Unknown tile encountered!");
                }
            }
        }
        // Swap pointers
        lumberyard_state* temp = current_lumberyard;
        current_lumberyard = next_lumberyard;
        next_lumberyard = temp;
        // Check for repeating state
        int resource_value = current_lumberyard->resource_value();
        if(hasElement(prev_resource_values, resource_value)) {
            repeats[resource_value] += 1;
        }
        prev_resource_values.push_back(resource_value);
        // Increment and display info.
        minutes += 1;
        current_lumberyard->print_ncurses(minutes);
        // sleep after each round.
        usleep(1000000/60);
    }

    // Stop ncurses.
    endwin();

    // Final reporting
    if(verbose) {
        std::cout << "Final Simulated State:" << std::endl;
        current_lumberyard->print(std::cout);
    }

    std::cout << "Simulated " << minutes << " minutes" << std::endl;

    if(repeats[*prev_resource_values.crbegin()] == total_repeats) {
        auto prev_it = std::find(prev_resource_values.crbegin()+1,prev_resource_values.crend(), *prev_resource_values.crbegin());
        auto period = std::distance(prev_resource_values.crbegin(), prev_it);
        std::cout << "Period is: " << period << std::endl;
        // Copy period values.
        int pattern[period];
        for(int idx = 0; idx < period; ++idx) {
            pattern[idx] = prev_resource_values[prev_resource_values.size()-1-period+idx];
        }
        int period_start = prev_resource_values.size()-1;
        std::cout << "Final Resource Value is: " << pattern[(num_minutes-period_start)%period] << std::endl;
    } else {
        std::cout << "Final Resource Value is: " << current_lumberyard->resource_value() << std::endl;
    }

	return 0;
}
