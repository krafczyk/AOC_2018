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
class position;

template<typename Iter>
void print_map_with_markers_helper(const game_map& map, const std::vector<entity>& entities, const Iter& begin, const Iter& end, char tile);
template<typename C>
void print_map_with_markers(const game_map& map, const std::vector<entity>& entities, const C& positions, char tile);

class direction {
	public:
		direction(dir_t d=0) {
			this->d = d;
		}
		direction(const direction& rhs) = default;
		direction(direction&& rhs) = default;
		direction& operator=(const direction& rhs) = default;
		bool operator==(const direction& rhs) const {
			if(this->d == rhs.d) {
				return true;
			} else {
				return false;
			}
		}
		bool operator!=(const direction& rhs) const {
			if(this->d != rhs.d) {
				return true;
			} else {
				return false;
			}
		}
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

static const direction east(0);
static const direction north(1);
static const direction west(2);
static const direction south(3);

class position {
	public:
		position() {
			this->line_idx = 0;
			this->x_idx = 0;
		}
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
		position(const position& rhs) = default;
		position(position&& rhs) = default;
		position& operator=(const position& rhs) = default;
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
		position operator+(const direction& rhs) const {
			return (*this)+position(rhs);
		}
		position operator-(const direction& rhs) const {
			return (*this)-position(rhs);
		}
		bool operator<(const position& rhs) const {
			if(this->line_idx < rhs.line_idx) {
				return true;
			} else if(this->line_idx == rhs.line_idx) {
				if(this->x_idx < rhs.x_idx) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
		pos_idx_t get_line_idx() const {
			return this->line_idx;
		}
		pos_idx_t get_x_idx() const {
			return this->x_idx;
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
		std::string get_str() const {
			std::stringstream ss;
			ss << this->x_idx << "," << this->line_idx;
			return ss.str();
		}
	private:
		pos_idx_t line_idx;
		pos_idx_t x_idx;
};

// Implementation of A*?
void find_fastest_path(const game_map& map [[maybe_unused]], const std::vector<entity>& entities [[maybe_unused]], direction& dir [[maybe_unused]], pos_idx_t& cost [[maybe_unused]], const position& A, const position& B) {
	std::cout << "Finding shortest path between " << A.get_str() << " and " << B.get_str() << std::endl;
}

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
		char get_tile(const position& pos) const {
			return (*this)(pos.get_line_idx(), pos.get_x_idx());
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
		entity(pos_idx_t line_idx, pos_idx_t x_idx, char team, int id) {
			pos = position(line_idx, x_idx);
			this->team = team;
			this->hp = 200;
			this->ap = 3;
			this->id = id;
		}
		position get_pos() const {
			return this->pos;
		}
		char get_team() const {
			return this->team;
		}
		int get_id() const {
			return this->id;
		}
		void take_turn(const game_map& map, std::vector<entity>&entities) {
			this->move(map, entities);
			this->attack(entities);
		}
		void move(const game_map& map, std::vector<entity>&entities) {
			// come up with positions to get to
			std::set<position> candidates;
			for(size_t e_idx = 0; e_idx < entities.size(); ++e_idx) {
				if(entities[e_idx].get_id() != this->get_id()) {
					if (entities[e_idx].get_team() != this->get_team()) {
						direction dir = east;
						do {
							position new_candidate = entities[e_idx].get_pos()+dir;
							if(map.get_tile(new_candidate) == '.') {
								// Check that no entity overlaps here
								bool no_entity = true;
								for(size_t e_idx_2 = 0; e_idx_2 < entities.size(); ++e_idx_2) {
									if(new_candidate == entities[e_idx_2].get_pos()) {
										no_entity = false;
										break;
									}
								}
								if(no_entity) {
									candidates.insert(new_candidate);
								}
							}
							// Advance direction
							dir.turn_cw();
						} while(dir != east);
					}
				}
			}
			// Verify that candidates are correct
			//print_map_with_markers(map, entities, candidates, '?');

			// --- Calculating paths and costs ---
			// Calculate cost and movement direction to each candidate
			// Create temp infrastructure
			class path_cost {
				public:
					path_cost(const pos_idx_t cost, const direction first_step, const position destination) {
						this->cost = cost;
						this->first_step = first_step;
						this->destination = destination;
					}
					pos_idx_t cost;
					direction first_step;
					position destination;
			};
			std::vector<path_cost> path_candidates;
			for(auto candidate_it=candidates.cbegin(); candidate_it != candidates.cend(); ++candidate_it) {
				direction first_step;
				pos_idx_t cost;
				find_fastest_path(map, entities, first_step, cost, this->get_pos(), *candidate_it);
				//path_candidates.push_back(path_cost(cost, first_step, *candidate_it));
			}
		}
		void attack(std::vector<entity>& entities [[maybe_unused]]) {
		}

	private:
		position pos;
		char team;
		int hp;
		int ap;
		int id;
};

void print_map(const game_map& map, const std::vector<entity>& entities) {
	for(pos_idx_t line_idx = 0; line_idx < map.get_num_lines(); ++line_idx) {
		for(pos_idx_t idx = 0; idx < map.get_line_len(); ++idx) {
			char the_char = map(line_idx, idx);
			position current_pos(line_idx, idx);
			for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
				if(current_pos == entity_it->get_pos()) {
					the_char = entity_it->get_team();
				}
			}
			std::cout << the_char;
		}
		std::cout << std::endl;
	}
}

// This is really disgusting. I wish concepts was in clang already
template<typename ConstIter>
void print_map_with_markers_helper(const game_map& map, const std::vector<entity>& entities, const ConstIter& begin, const ConstIter& end, char tile) {
	for(size_t line_idx=0; line_idx < map.get_num_lines(); ++line_idx) {
		for(size_t x_idx = 0; x_idx < map.get_line_len(); ++x_idx) {
			position current_pos(line_idx, x_idx);
			char the_char = map.get_tile(current_pos);
			for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
				if(current_pos == entity_it->get_pos()) {
					the_char = entity_it->get_team();
				}
			}
			ConstIter it = begin;
			while(it != end) {
				if((*it) == current_pos) {
					the_char = tile;
				}
				++it;
			}
			std::cout << the_char;
		}
		std::cout << std::endl;
	}
}

// This is really disgusting. I wish concepts was in clang already
template<typename C>
void print_map_with_markers(const game_map& map, const std::vector<entity>& entities, const C& positions, char tile) {
	print_map_with_markers_helper(map, entities, positions.cbegin(), positions.cend(), tile);
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
				entities.push_back(entity(line_idx, x_idx, tile_char, entities.size()));
				tile_char = '.';
			}
			map.assign(line_idx, x_idx) = tile_char;
		}
	}

	if (verbose) {
		std::cout << "Initial state" << std::endl;
		print_map(map, entities);
	}

	entities[0].take_turn(map, entities);

	return 0;
}
