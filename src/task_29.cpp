#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef size_t pos_idx_t;
typedef int dir_t;

class game_map;
class entity;

class direction {
	public:
		direction(dir_t d) {
			this->d = d;
		}
		direction(direction& rhs) = default;
		direction(direction&& rhs) = default;
		void turn_cw() {
			this->d -= 1;
			if(this->d < 0) {
				this->d += direction::num_dirs;
			}
			this->d = this->d%direction::num_dirs;
		}
		void turn_ccw() {
			this->d += 1;
			this->d = this->d%direction::num_dirs;
		}
		dir_t get_d() const {
			return this->d;
		}

	private:
		dir_t d;
		static const dir_t num_dirs = 4;
};

class position {
	public:
		position(pos_idx_t line_idx, pos_idx_t x_idx) {
			this->line_idx = line_idx;
			this->x_idx = x_idx;
		}
		position(const direction& dir) {
			dir_t d = dir.get_d();
			if(d == 0) {
				this->line_idx = 0;
				this->x_idx = 1;
			} else if(d == 1) {
				this->line_idx = 1;
				this->x_idx = 0;
			} else if(d == 2) {
				this->line_idx = 0;
				this->x_idx = -1;
			} else {
				this->line_idx = -1;
				this->x_idx = 0;
			}
		}
		position(position& rhs) = default;
		position(position&& rhs) = default;
		bool operator==(const position& rhs) const {
			if((this->line_idx == rhs.line_idx)&&(this->x_idx == rhs.x_idx)) {
				return true;
			} else {
				return false;
			}
		}
		bool operator!=(const position& rhs) const {
			return (!((*this)==rhs));
		}
		position operator*(pos_idx_t a) const {
			return position(this->line_idx*a, this->x_idx*a);
		}
		position operator+(const position& rhs) const {
			return position(this->line_idx+rhs.line_idx, this->x_idx+rhs.x_idx);
		}
		position operator-(const position& rhs) const {
			return position(this->line_idx-rhs.line_idx, this->x_idx-rhs.x_idx);
		}
		pos_idx_t dist(const position& in) const {
			pos_idx_t line_diff = 0;
			if(this->line_idx > in.line_idx) {
				line_diff = this->line_idx-in.line_idx;
			} else {
				line_diff = in.line_idx-this->line_idx;
			}
			pos_idx_t x_diff = 0;
			if(this->x_idx > in.x_idx) {
				x_diff = this->x_idx-in.x_idx;
			} else {
				x_diff = in.x_idx-this->x_idx;
			}
			return line_diff+x_diff;
		}
	private:
		pos_idx_t line_idx;
		pos_idx_t x_idx;
};

// Implemenattion of A*?
//void find_fastest_path(direction& dir, pos_idx_t& cost, const position& A, const position& B) {
//}

class game_map {
	public:
		game_map(pos_idx_t num_lines, pos_idx_t line_len) {
			this->num_lines = num_lines;
			this->line_len = line_len;
			this->map = new char[this->num_lines*this->line_len];
			// Initialize map to 0.
			for(size_t idx = 0; idx < this->num_lines*this->line_len; ++idx) {
				this->map[idx] = 0;
			}
		}
		~game_map() {
			delete [] this->map;
		}
		char operator()(pos_idx_t line_idx, pos_idx_t idx) const {
			return this->map[this->line_len*line_idx+idx];
		}
		char& assign(pos_idx_t line_idx, pos_idx_t idx) {
			return this->map[this->line_len*line_idx+idx];
		}
		pos_idx_t get_num_lines() const {
			return this->num_lines;
		}
		pos_idx_t get_line_len() const {
			return this->line_len;
		}
		void print() const {
			for(pos_idx_t line_idx = 0; line_idx < this->num_lines; ++line_idx) {
				for(pos_idx_t idx = 0; idx < this->line_len; ++idx) {
					std::cout << (*this)(line_idx, idx);
				}
				std::cout << std::endl;
			}
		}
	private:
		pos_idx_t num_lines;
		pos_idx_t line_len;
		char* map;
};

class entity {
	public:
		entity(pos_idx_t line_idx, pos_idx_t x_idx, char team) {
			this->line_idx =line_idx;
			this->x_idx = x_idx;
			this->team = team;
		}
		pos_idx_t get_line_idx() const {
			return this->line_idx;
		}
		pos_idx_t get_x_idx() const {
			return this->x_idx;
		}
		char get_team() const {
			return this->team;
		}
	private:
		pos_idx_t line_idx;
		pos_idx_t x_idx;
		char team;
};

void print_map(const game_map& map, const std::vector<entity>& entities) {
	for(pos_idx_t line_idx = 0; line_idx < map.get_num_lines(); ++line_idx) {
		for(pos_idx_t idx = 0; idx < map.get_line_len(); ++idx) {
			char the_char = map(line_idx, idx);
			for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
				if((line_idx == entity_it->get_line_idx())&&(idx == entity_it->get_x_idx())) {
					the_char = entity_it->get_team();
				}
			}
			std::cout << the_char;
		}
		std::cout << std::endl;
	}

}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 29");
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

	// Load initial game state
	std::vector<std::string> input_data;
	std::string line;
	while(std::getline(infile, line)) {
		input_data.push_back(line);
	}

	game_map map(input_data.size(), input_data[0].size());
	std::vector<entity> entities;
	for(pos_idx_t line_idx = 0; line_idx < map.get_num_lines(); ++line_idx) {
		for(pos_idx_t x_idx = 0; x_idx < map.get_line_len(); ++x_idx) {
			char tile_char = input_data[line_idx][x_idx];
			if((tile_char == 'G')||tile_char == 'E') {
				entities.push_back(entity(line_idx, x_idx, tile_char));
				tile_char = '.';
			}
			map.assign(line_idx, x_idx) = tile_char;
		}
	}

	if (verbose) {
		std::cout << "Initial state" << std::endl;
		print_map(map, entities);
	}



	return 0;
}
