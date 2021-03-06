#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <deque>
#include <limits>
#include <algorithm>
#include <exception>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef size_t pos_idx_t;
typedef int dir_t;

class game_map;
class entity;
class position;

template<typename Iter>
void print_map_with_markers_helper(const game_map& map, const std::vector<entity>& entities, const Iter& begin, const Iter& end, char tile);
template<typename Iter>
void print_map_with_markers_helper(const game_map& map, const std::set<position>& forbidden, const Iter& begin, const Iter& end, char tile);
template<typename C>
void print_map_with_markers(const game_map& map, const std::vector<entity>& entities, const C& positions, char tile);
template<typename C>
void print_map_with_markers(const game_map& map, const std::set<position>& forbidden, const C& positions, char tile);

std::set<position> build_accessible_set(const game_map& map, const std::set<position>& forbidden, const position& start);
pos_idx_t find_closest_points(const game_map& map, const std::set<position>& forbidden, const position& A, const std::set<position>& destinations, std::set<position>& closest_set);

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
		std::set<position> neighbors() const {
			std::set<position> answer;
			answer.insert(position(this->line_idx+1, this->x_idx));
			answer.insert(position(this->line_idx, this->x_idx-1));
			answer.insert(position(this->line_idx, this->x_idx+1));
			answer.insert(position(this->line_idx-1, this->x_idx));
			return answer;
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
		int get_hp() const {
			return this->hp;
		}
		int get_ap() const {
			return this->ap;
		}
		bool take_turn(const game_map& map, std::vector<entity>&entities) {
			this->move(map, entities);
			return this->attack(entities);
		}
		void move(const game_map& map, std::vector<entity>&entities) {
			// Build fobidden set
			std::set<position> forbidden;
			for(auto it = entities.cbegin(); it != entities.cend(); ++it) {
				if (it->get_id() != this->get_id()) {
					forbidden.insert(it->get_pos());
				}
			}
			// come up with positions to get to
			std::set<position> candidates;
			for(size_t e_idx = 0; e_idx < entities.size(); ++e_idx) {
				if(entities[e_idx].get_id() != this->get_id()) {
					if (entities[e_idx].get_team() != this->get_team()) {
						std::set<position> neighbors = entities[e_idx].get_pos().neighbors();
						for(auto it = neighbors.cbegin(); it != neighbors.cend(); ++it) {
							// No walls
							if(map.get_tile(*it) == '#') {
								continue;
							}
							// No forbidden
							if(hasElement(forbidden, *it)) {
								continue;
							}
							candidates.insert(*it);
						}
					}
				}
			}

			// --- Check that we aren't already on a candidate
			for(auto it=candidates.cbegin(); it != candidates.cend(); ++it) {
				if(*it == this->get_pos()) {
					return;
				}
			}

			// Remove non reachable candidates
			std::set<position> reachablePoints = build_accessible_set(map, forbidden, this->get_pos());

			for(auto it = candidates.begin(); it != candidates.end(); ) {
				if(std::find(reachablePoints.cbegin(), reachablePoints.cend(), *it) == reachablePoints.cend()) {
					it = candidates.erase(it);
				} else {
					++it;
				}
			}

			// Quit early if there are no candidates
			if(candidates.size() == 0) {
				return;
			}

			std::set<position> closest_set;
			pos_idx_t dist = find_closest_points(map, forbidden, this->get_pos(), candidates, closest_set);

			if(closest_set.size() == 0) {
				throw std::runtime_error("This shouldn't happen!");
			}

			// Choose earliest by reading order.
			position choice = *closest_set.begin();

			std::set<position> neighbors = this->pos.neighbors();
			for(auto it = neighbors.begin(); it != neighbors.end(); ++it) {
				if(map.get_tile(*it) == '#') {
					continue;
				}
				bool entity = false;
				for(auto e_it = entities.cbegin(); e_it != entities.cend(); ++e_it) {
					if(e_it->get_pos() == *it) {
						entity = true;
					}
				}
				if(entity) {
					continue;
				}
				std::set<position> choice_set;
				choice_set.insert(choice);
				pos_idx_t dist_2 = find_closest_points(map, forbidden, *it, choice_set, closest_set);
				if(closest_set.size() > 0) {
					if(dist_2 == dist-1) {
						// Set position here!
						this->pos = *it;
						break;
					}
				}
			}
		}
		bool attack(std::vector<entity>& entities) {
			typedef std::pair<position, size_t> target_info;
			// Are there any enemies left?
			bool attack_over = true;
			for(size_t idx = 0; idx < entities.size(); ++idx) {
				if (entities[idx].get_team() != this->get_team()) {
					attack_over = false;
					break;
				}
			}
			if(attack_over) {
				return false;
			}
			std::set<target_info> targets;
			int lowest_health = std::numeric_limits<int>::max();

			std::set<position> neighbors = this->pos.neighbors();
			for(auto n_it = neighbors.cbegin(); n_it != neighbors.cend(); ++n_it) {
				for(size_t entity_idx = 0; entity_idx != entities.size(); ++entity_idx) {
					entity& the_entity = entities[entity_idx];
					// Don't attack yourself silly!
					if(the_entity.get_id() != this->get_id()) {
						// Don't attack allies silly!
						if(the_entity.get_team() != this->get_team()) {
							if(the_entity.get_pos() == *n_it) {
								if(the_entity.get_hp() < lowest_health) {
									targets.clear();
									targets.insert(target_info(entities[entity_idx].get_pos(), entity_idx));
									lowest_health = the_entity.get_hp();
								} else if (the_entity.get_hp() == lowest_health) {
									targets.insert(target_info(entities[entity_idx].get_pos(), entity_idx));
								}
							}
						}
					}
				}
			}

			if(targets.size() != 0) {
				// Attack first target
				entities[targets.begin()->second].hp -= this->ap;
			}
			return true;
		}

	private:
		position pos;
		char team;
		int hp;
		int ap;
		int id;
};

pos_idx_t find_closest_points(const game_map& map, const std::set<position>& forbidden, const position& A, const std::set<position>& destinations, std::set<position>& closest_set) {
	typedef std::pair<position,pos_idx_t> node_summary;
	std::vector<node_summary> openSet;
	openSet.push_back(node_summary(A, 0));
	std::set<position> seen;
	closest_set.clear();

	pos_idx_t dist = 0;
	bool found_dist = false;
	while(openSet.size() != 0) {
		node_summary considered = *openSet.begin();
		openSet.erase(openSet.begin());
		if (found_dist && (considered.second > dist)) {
			return dist;
		}

		if(hasElement(seen, considered.first)) {
			throw std::runtime_error("This shouldn't happen");
		}
		seen.insert(considered.first);
		if(hasElement(destinations, considered.first)) {
			found_dist = true;
			dist = considered.second;
			closest_set.insert(considered.first);
		}
		std::set<position> neighbors = considered.first.neighbors();
		for(auto n_it = neighbors.cbegin(); n_it != neighbors.cend(); ++n_it) {
			// Don't add seen elements
			if(hasElement(seen, *n_it)) {
				continue;
			}
			position neighbor = *n_it;
			if(std::find_if(openSet.cbegin(), openSet.cend(), [&neighbor](const node_summary& in) {return (neighbor == in.first); }) != openSet.cend()) {
				continue;
			}
			if(map.get_tile(*n_it) == '#') {
				continue;
			}
			bool collision = false;
			for(auto it = forbidden.cbegin(); it != forbidden.cend(); ++it) {
				if(*it == *n_it) {
					collision = true;
					break;
				}
			}
			if(collision) {
				continue;
			}
			openSet.push_back(node_summary(*n_it,considered.second+1));
		}
	}
	if(found_dist) {
		return dist;
	}
	throw std::runtime_error("We should never be here");
	return 0;
}

std::set<position> build_accessible_set(const game_map& map, const std::set<position>& forbidden, const position& start) {
	std::set<position> openSet;
	openSet.insert(start);

	std::set<position> reachedSet;

	while(openSet.size() != 0) {
		position current = *openSet.begin();
		openSet.erase(std::find(openSet.begin(), openSet.end(), current));

		reachedSet.insert(current);

		std::set<position> neighbors = current.neighbors();
		for(auto n_it = neighbors.cbegin(); n_it != neighbors.cend(); ++n_it) {
			// Is this a wall?
			if(map.get_tile(*n_it) == '#') {
				continue;
			}
			// Is this an entity?
			if(hasElement(forbidden, *n_it)) {
				continue;
			}
			// We already looked at this node.
			if(std::find(reachedSet.cbegin(), reachedSet.cend(), *n_it) != reachedSet.cend()) {
				continue;
			}
			// Discover a new node.
			openSet.insert(*n_it);
		}
	}

	return reachedSet;
}

void print_map(const game_map& map, const std::vector<entity>& entities) {
	for(pos_idx_t line_idx = 0; line_idx < map.get_num_lines(); ++line_idx) {
		typedef std::pair<char,int> ent_summary;
		std::vector<ent_summary> entity_summary;
		for(pos_idx_t idx = 0; idx < map.get_line_len(); ++idx) {
			char the_char = map(line_idx, idx);
			position current_pos(line_idx, idx);
			for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
				if(current_pos == entity_it->get_pos()) {
					the_char = entity_it->get_team();
					entity_summary.push_back(ent_summary(the_char, entity_it->get_hp()));
				}
			}
			std::cout << the_char;
		}
		if(entity_summary.size() > 0) {
			std::cout << " ";
			for(size_t idx = 0; idx < entity_summary.size(); ++idx) {
				std::cout << entity_summary[idx].first << "(" << entity_summary[idx].second << "), ";
			}
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

template<typename ConstIter>
void print_map_with_markers_helper(const game_map& map, const std::set<position>& forbidden, const ConstIter& begin, const ConstIter& end, char tile) {
	for(size_t line_idx=0; line_idx < map.get_num_lines(); ++line_idx) {
		for(size_t x_idx = 0; x_idx < map.get_line_len(); ++x_idx) {
			position current_pos(line_idx, x_idx);
			char the_char = map.get_tile(current_pos);
			for(auto f_it = forbidden.cbegin(); f_it != forbidden.cend(); ++f_it) {
				if(current_pos == *f_it) {
					the_char = 'x';
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

template<typename C>
void print_map_with_markers(const game_map& map, const std::set<position>& forbidden, const C& positions, char tile) {
	print_map_with_markers_helper(map, forbidden, positions.cbegin(), positions.cend(), tile);
}



int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	bool super_verbose = false;
	int test_val = 0;
	bool test_val_passed = false;
	size_t num_moves = 0;
	bool progress = false;
	ArgParse::ArgParser Parser("Task 29");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-n/--num-moves", "Number of moves", &num_moves, ArgParse::Argument::Optional);
	Parser.AddArgument("-t/--test-val", "Expected value", &test_val, ArgParse::Argument::Optional, &test_val_passed);
	Parser.AddArgument("-p/--progress", "Show progress of battle", &progress);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
	Parser.AddArgument("-sv/--super-verbose", "Print Super Verbose output", &super_verbose);

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
	std::string line = "";
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

	std::set<char> unique_teams;
	for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
		unique_teams.insert(entity_it->get_team());
	}
	size_t steps = 0;
	size_t last_full_step = 0;
	bool combat_finished = false;
	while((!combat_finished)&&((num_moves == 0)||(steps < num_moves))) {
		// We first determine the turn order for the whole step.
		typedef std::pair<position,int> turn_order;
		std::set<turn_order> need_turn;
		// Fill need_turn with all entities
		for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
			need_turn.insert(turn_order(entity_it->get_pos(),entity_it->get_id()));
		}
		while(need_turn.size() > 0) {
			// Order entities
			int entity_id = need_turn.begin()->second;
			// Find the entity;
			auto entity_it = std::find_if(entities.begin(), entities.end(), [entity_id](const entity& a) { return (a.get_id() == entity_id);});
			if(entity_it == entities.end()) {
				// Entity was killed and removed!!
				need_turn.erase(need_turn.begin());
				continue;
			}
			// Take a turn
			if(!entity_it->take_turn(map,entities)) {
				last_full_step = steps;
				combat_finished = true;
				break;
			}
			// Remove from the need turn counter
			need_turn.erase(need_turn.begin());
			// Detect whether any entity has died.
			int dead_unit = -1;
			for(size_t idx = 0; idx < entities.size(); ++idx) {
				if(entities[idx].get_hp() <= 0) {
					dead_unit = entities[idx].get_id();
					break;
				}
			}
			// Remove a dead unit if necessary.
			if(dead_unit >= 0) {
				entities.erase(std::find_if(entities.begin(), entities.end(), [dead_unit](const entity& a) { return (a.get_id() == dead_unit); }));
				// Remove dead unit from need_turn if it's in there
				auto dead_need_turn_it [[maybe_unused]] = std::find_if(need_turn.begin(), need_turn.end(), [dead_unit](const turn_order& a) {return (a.second == dead_unit);});
				if(dead_need_turn_it != need_turn.end()) {
					need_turn.erase(dead_need_turn_it);
				}
			}
		}
		// Count remaining unique teams
		unique_teams.clear();
		for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
			unique_teams.insert(entity_it->get_team());
		}
		// Show progress
		if(progress&&(steps%20 == 0)) {
			std::map<char,int> summary;
			std::map<char,int> num;
			for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
				summary[entity_it->get_team()] += entity_it->get_hp();
				num[entity_it->get_team()] += 1;
			}
			std::cout << "After " << steps << " Steps:";
			for(auto summary_it = summary.cbegin(); summary_it != summary.cend(); ++summary_it) {
				std::cout << " " << summary_it->first << ": " << num[summary_it->first] << " " << summary_it->second;
			}
			std::cout << std::endl;
		}
		++steps;
		if(super_verbose) {
			std::cout << "----- After Step " << steps << " -----" << std::endl;
			print_map(map, entities);
		}
	}
	if(verbose|super_verbose) {
		std::cout << "----- After Step " << steps << " -----" << std::endl;
		print_map(map, entities);
	}

	int hp_sum = 0;
	for(size_t idx = 0; idx < entities.size(); ++idx) {
		hp_sum += entities[idx].get_hp();
	}

	std::map<char,std::string> name_map;
	name_map['G'] = "Goblins";
	name_map['E'] = "Elves";

	std::cout << "Combat ends after " << last_full_step << " full rounds" << std::endl;
	std::cout << name_map[entities[0].get_team()] << " win with " << hp_sum << " total hit points" <<  std::endl;
	std::cout << "The outcome is: " << last_full_step*hp_sum << std::endl;

	if(test_val_passed) {
		if((int) (steps-1)*hp_sum == test_val) {
			std::cout << "Test Passed!" << std::endl;
		} else {
			std::cout << "Test Failed!" << std::endl;
		}
	}

	return 0;
}
