#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
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
template<typename C>
void print_map_with_markers(const game_map& map, const std::vector<entity>& entities, const C& positions, char tile);

std::vector<position> A_Star(const game_map& map [[maybe_unused]], const std::vector<entity>& entities [[maybe_unused]], const position& A, const position& B);
std::vector<position> find_shortest_path(const game_map& map, const std::vector<entity>& entities, const position& A, const position& B, const std::set<position>& visited, int stack_level);
std::set<position> build_accessible_set(const game_map& map, const std::vector<entity>& entities, const position& start);

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
			// come up with positions to get to
			typedef std::pair<pos_idx_t,position> candidate_holder;
			std::set<candidate_holder> candidates;
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
									// Need to allow candidates to overlap with self.
									if(entities[e_idx_2].get_id() != this->get_id()) {
										if(new_candidate == entities[e_idx_2].get_pos()) {
											no_entity = false;
											break;
										}
									}
								}
								if(no_entity) {
									candidates.insert(candidate_holder(this->get_pos().dist(new_candidate),new_candidate));
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

			// --- Check that we aren't already on a candidate
			for(auto candidate_it=candidates.cbegin(); candidate_it != candidates.cend(); ++candidate_it) {
				if(candidate_it->second == this->get_pos()) {
					//std::cout << "Already on a candidate" << std::endl;
					return;
				}
			}

			// Remove non reachable candidates
			std::set<position> reachablePoints = build_accessible_set(map, entities, this->get_pos());
			//std::cout << "Points reachable from " << this->get_pos().get_str() << std::endl;
			//print_map_with_markers(map, entities, reachablePoints, 'r');

			for(auto candidate_it = candidates.begin(); candidate_it != candidates.end(); ) {
				if(std::find(reachablePoints.cbegin(), reachablePoints.cend(), candidate_it->second) == reachablePoints.cend()) {
					candidate_it = candidates.erase(candidate_it);
				} else {
					++candidate_it;
				}
			}

                        // --- Calculating paths and costs ---
                        // Calculate cost and movement direction to each candidate
                        // Create temp infrastructure
                        typedef std::pair<position,std::vector<position>> path_def;
                        std::set<path_def> path_candidates;
                        size_t best_length = std::numeric_limits<size_t>::max();
                        //std::cout << "Current Position " << this->get_pos().get_str() << std::endl;
                        //std::vector<position> temp;
                        //temp.push_back(this->get_pos());
                        //print_map_with_markers(map, entities, temp, 'x');
                        for(auto candidate_it=candidates.cbegin(); candidate_it != candidates.cend(); ++candidate_it) {
                                std::vector<position> path = A_Star(map, entities, this->get_pos(), candidate_it->second);
                                // Skip unreachable points
                                if(path.size() == 0) {
                                        continue;
                                }
                                //std::cout << "Path Candidate: " << path.size() << " to " << candidate_it->get_str() << std::endl;
                                //for(size_t p_it = 0; p_it < path.size(); ++p_it) {
                                //      std::cout << path[p_it].get_str() << std::endl;
                                //}
                                if(path.size() < best_length) {
                                        path_candidates.clear();
                                        path_candidates.insert(path_def(candidate_it->second, path));
                                        best_length = path.size();
                                } else if (path.size() == best_length) {
                                        path_candidates.insert(path_def(candidate_it->second, path));
                                }
                        }
                        if(path_candidates.size() == 0) {
                                // No candidate is reachable!
                                return;
                        }
                        //std::cout << "Best Paths: Num: " << path_candidates.size() << std::endl;
                        //for(auto it = path_candidates.begin(); it != path_candidates.end(); ++it) {
                        //      std::cout << "Candidate: " << it->first.get_str() << std::endl;
                        //      for(size_t p_it = 0; p_it < it->second.size(); ++p_it)
                        //              std::cout << it->second[p_it].get_str() << std::endl;
                        //      }
                        //      print_map_with_markers(map, entities, it->second, 'P');
                        //}
                        // Pick first path and advance position!
                        this->pos = path_candidates.begin()->second[0];
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
			direction dir = east;
			do {
				// Check each neighbor
				position neighbor = this->pos+dir;
				// Turn to next direction.
				dir.turn_cw();

				for(size_t entity_idx = 0; entity_idx != entities.size(); ++entity_idx) {
					entity& the_entity = entities[entity_idx];
					// Don't attack yourself silly!
					if(the_entity.get_id() != this->get_id()) {
						// Don't attack allies silly!
						if(the_entity.get_team() != this->get_team()) {
							if(the_entity.get_pos() == neighbor) {
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
			} while(dir != east);

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

std::vector<position> find_shortest_path(const game_map& map, const std::vector<entity>& entities, const position& A, const position& B, const std::set<position>& visited, int stack_level) {
	std::vector<position> path;
	// build neighbor candidates
	std::set<position> neighbors;

	//std::cout << "find_shortest_path: Start: " << A.get_str() << " Level: " << stack_level << std::endl;
	//if(stack_level > 2) {
	//	throw std::runtime_error("Stop early stack too deep!");
	//}
	//std::cout << "Start Loop A" << std::endl;
	direction dir = east;
	do {
		//std::cout << "Loop A" << std::endl;
		// Check each neighbor
		position neighbor = A+dir;
		// Turn to next direction.
		dir.turn_cw();

		// Determine if it's a wall
		if(map.get_tile(neighbor) == '#') {
			continue;
		}
		// Determine if it's been visited
		if(hasElement(visited, neighbor)) {
			continue;
		}
		// Determine if it has an entity on it
		bool collision = false;
		for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
			if(entity_it->get_pos() == neighbor) {
				collision = true;
				break;
			}
		}
		if(collision) {
			continue;
		}

		// These neighbors are good candidates
		neighbors.insert(neighbor);
		//std::cout << "Neighbor candidate: " << neighbor.get_str() << std::endl;

	} while(dir != east);
	//std::cout << "End Loop A" << std::endl;

	if(neighbors.size() == 0) {
		// No path forward..
		return path;
	}

	// We now need to choose one of the neighbors.
	typedef std::pair<position,std::vector<position>> path_summary;
	std::set<path_summary> best_paths;
	pos_idx_t best_cost = std::numeric_limits<pos_idx_t>::max();

	//std::cout << "Start Loop B" << std::endl;
	while(neighbors.size() != 0) {
		//std::cout << "Loop B" << std::endl;
		// Find neighbor which has best estimated score.
		// Score must be better at least as good as previously recorded concrete paths.

		// Create neighbor sublist with least distance estimate
		std::set<position> closest_neighbors;
		pos_idx_t lowest_estimate = std::numeric_limits<pos_idx_t>::max();
		for(auto n_it = neighbors.cbegin(); n_it != neighbors.cend(); ++n_it) {
			pos_idx_t n_dist = n_it->dist(B);
			if(n_dist < lowest_estimate) {
				closest_neighbors.clear();
				closest_neighbors.insert(*n_it);
				lowest_estimate = n_dist;
			} else if (n_dist == lowest_estimate) {
				closest_neighbors.insert(*n_it);
			}
		}

		if (lowest_estimate == 0) {
			// We've found the goal! Quit here!
			path.push_back(*closest_neighbors.begin());
			return path;
		}

		// Quit early if we've already found a better path than these are estimated to be
		if (lowest_estimate > best_cost) {
			break;
		}

		// Pick the closest neighbor who comes first
		position picked_neighbor = *closest_neighbors.begin();

		// Remove from considered neighbors.
		neighbors.erase(std::find(neighbors.begin(), neighbors.end(), picked_neighbor));

		std::set<position> new_visited = visited;
		new_visited.insert(A);
		std::vector<position> n_path = find_shortest_path(map, entities, picked_neighbor, B, new_visited, stack_level+1);
		if(n_path.size() == 0) {
			// We don't add empty paths.. i.e. this path couldn't reach the destination.
			continue;
		} else {
			if (n_path.size() < best_cost) {
				best_paths.clear();
				best_paths.insert(path_summary(picked_neighbor, n_path));
				best_cost = n_path.size();
			} else if (n_path.size() == best_cost) {
				best_paths.insert(path_summary(picked_neighbor, n_path));
			}
		}
	}
	//std::cout << "End Loop B" << std::endl;
	// We should have a set of best paths
	if(best_paths.size() == 0) {
		// No paths to solution..
		return path;
	} else {
		// With tied paths, always choose the 'first' one.
		auto best_path = best_paths.begin();
		path.push_back(best_path->first);
		path.insert(path.end(), best_path->second.begin(), best_path->second.end());
		return path;
	}
}

// Implementation of A*?
std::vector<position> A_Star(const game_map& map [[maybe_unused]], const std::vector<entity>& entities [[maybe_unused]], const position& A, const position& B) {
	bool super_verbose = false;
	std::vector<position> path;

	std::set<position> openSet;
	openSet.insert(A);

	std::set<position> closedSet;

	// The cost of getting from the start to the given node.
	// This is a running tally
	std::map<position,pos_idx_t> gScore;
	gScore[A] = 0; // 0 cost to get from the start to the start

	// The cost of going from the given node to the goal.
	// Partly known, partly heuristic.
	std::map<position,pos_idx_t> fScore;
	gScore[A] = A.dist(B); // purely heuristic

	// Tracking the most efficient from directions
	std::map<position,std::set<position>> cameFrom;

	if(super_verbose) {
	std::cout << "Going from " << A.get_str() << " to " << B.get_str() << std::endl;
	}

	while(openSet.size() != 0) {
		if(super_verbose) {
		// Report the open set
		std::cout << "--Open Set--" << std::endl;
		print_map_with_markers(map, entities, openSet, 'o');

		std::cout << "--Closed Set--" << std::endl;
		print_map_with_markers(map, entities, closedSet, 'c');
		}
		// Pick a current position from the open set. which minimizes fScore.
		std::set<position> current_possibilities;
		pos_idx_t min_fscore = std::numeric_limits<pos_idx_t>::max();
		for(auto it = openSet.cbegin(); it != openSet.cend(); ++it) {
			if(fScore[*it] < min_fscore) {
				current_possibilities.clear();
				current_possibilities.insert(*it);
				min_fscore = fScore[*it];
			} else if(fScore[*it] == min_fscore) {
				current_possibilities.insert(*it);
			}
		}

		if(super_verbose) {
		std::cout << "current possibilities" << std::endl;
		print_map_with_markers(map, entities, current_possibilities, 'p');
		}

		// Possibility: rank also by gScore.
		position current = *current_possibilities.begin();

		if(current == B) {
			path.push_back(B);
			while(std::find(cameFrom[path[0]].cbegin(), cameFrom[path[0]].cend(), A) == cameFrom[path[0]].cend()) {
				if(super_verbose) {
				std::cout << "Can reach " << path[0].get_str() << " from:" << std::endl;
				for(auto it = cameFrom[path[0]].cbegin(); it != cameFrom[path[0]].cend(); ++it) {
					std::cout << it->get_str() << std::endl;

				}
				}
				path.insert(path.begin(), *cameFrom[path[0]].begin());
			}
			if(super_verbose) {
			std::cout << "Found Path" << std::endl;
			print_map_with_markers(map, entities, path, 'P');
			}
			return path;
		}

		// Put it in the closed set
		closedSet.insert(current);
		// Remove it from the open set
		openSet.erase(std::find(openSet.begin(), openSet.end(), current));

		// Go through neighbors of current and add them to the open set if they aren't in either set already.
		direction dir = east;
		do {
			position neighbor = current+dir;
			// Advance direction
			dir.turn_cw();

			// Check whether neighbor is in the closedSet
			if(std::find(closedSet.cbegin(), closedSet.cend(), neighbor) != closedSet.cend()) {
				continue;
			}
			// Check whether neighbor is a wall or entity
			if(map.get_tile(neighbor) == '#') {
				continue;
			}
			bool collision = false;
			for(auto it = entities.cbegin(); it != entities.cend(); ++it) {
				if(it->get_pos() == neighbor) {
					collision = true;
					break;
				}
			}
			if(collision) {
				continue;
			}

			pos_idx_t this_gscore = gScore[current] + 1;
			if(std::find(openSet.cbegin(), openSet.cend(), neighbor) == openSet.cend()) {
				// Discover a new node
				openSet.insert(neighbor);
				gScore[neighbor] = this_gscore;
				fScore[neighbor] = this_gscore+neighbor.dist(B);
				cameFrom[neighbor].insert(current);
			} else {
				// This is worse than another path we've found.
				if(this_gscore > gScore[neighbor]) {
					continue;
				}
				if(this_gscore == gScore[neighbor]) {
					cameFrom[neighbor].insert(current);
				} else {
					gScore[neighbor] = this_gscore;
					fScore[neighbor] = this_gscore+neighbor.dist(B);
					cameFrom[neighbor].clear();
					cameFrom[neighbor].insert(current);
				}
			}
		} while(dir != east);
	}
	// No path to goal.
	return path;
}

std::set<position> build_accessible_set(const game_map& map, const std::vector<entity>& entities, const position& start) {
	std::set<position> openSet;
	openSet.insert(start);

	std::set<position> reachedSet;

	while(openSet.size() != 0) {
		position current = *openSet.begin();
		openSet.erase(std::find(openSet.begin(), openSet.end(), current));

		reachedSet.insert(current);

		direction dir = east;
		do {
			position neighbor = current+dir;
			dir.turn_cw();

			// Is this a wall?
			if(map.get_tile(neighbor) == '#') {
				continue;
			}
			// Is this an entity?
			bool collision = false;
			for(auto entity_it = entities.cbegin(); entity_it != entities.cend(); ++entity_it) {
				if(entity_it->get_pos() == neighbor) {
					collision = true;
				}
			}
			if(collision) {
				continue;
			}
			// We already looked at this node.
			if(std::find(reachedSet.cbegin(), reachedSet.cend(), neighbor) != reachedSet.cend()) {
				continue;
			}

			// Discover a new node.
			openSet.insert(neighbor);
		} while (dir != east);
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

// This is really disgusting. I wish concepts was in clang already
template<typename C>
void print_map_with_markers(const game_map& map, const std::vector<entity>& entities, const C& positions, char tile) {
	print_map_with_markers_helper(map, entities, positions.cbegin(), positions.cend(), tile);
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
		// Remove dead units
		//for(auto entity_it = entities.begin(); entity_it != entities.end();) {
		//	if(entity_it->get_hp() <= 0) {
		//		entity_it = entities.erase(entity_it);
		//	} else {
		//		++entity_it;
		//	}
		//}
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
