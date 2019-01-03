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

typedef int p_idx;

void print_options(const std::vector<std::vector<std::vector<std::string>>>& options, std::ostream& out) {
    for(auto it_1 = options.cbegin(); it_1 != options.cend(); ++it_1) {
        out << "Layer 1 item" << std::endl;
        for(auto it_2 = it_1->cbegin(); it_2 != it_1->cend(); ++it_2) {
            out << "Layer 2 item" << std::endl;
            for(auto it_3 = it_2->cbegin(); it_3 != it_2->cend(); ++it_3) {
                out << *it_3 << std::endl;
            }
        }
    }
}

void print_options(const std::vector<std::vector<std::string>>& options, std::ostream& out) {
    for(auto it_1 = options.cbegin(); it_1 != options.cend(); ++it_1) {
        out << "Layer 1 item" << std::endl;
        for(auto it_2 = it_1->cbegin(); it_2 != it_1->cend(); ++it_2) {
            out << *it_2 << std::endl;
        }
    }
}

std::vector<std::string> combine(std::vector<std::vector<std::string>>& list) {
    //std::cout << "Combine called: " << std::endl;
    //print_options(list, std::cout);
    std::vector<std::string> results;
    for(auto list_it = list.begin(); list_it != list.end(); ++list_it) {
        std::vector<std::string> new_results;
        if(results.size() > 0) {
            for(size_t o_idx = 0; o_idx < results.size(); ++o_idx) {
                //std::cout << "level 1" << std::endl;
                for(size_t l_idx = 0; l_idx < list_it->size(); ++l_idx) {
                    //std::cout << "level 2" << std::endl;
                    // Concatenate.
                    new_results.push_back(results[o_idx]+(*list_it)[l_idx]);
                }
            }
        } else {
            //std::cout << "Initially empty!" << std::endl;
            //ConstForEach(*list_it, [](const std::string& in) {
            //    std::cout << in << std::endl;
            //});
            std::swap(new_results, *list_it);
        }
        std::swap(results, new_results);
    }
    return results;
}

void remove_duplicates(std::vector<std::string>& in) {
    for(auto it = in.begin(); it != in.end(); ++it) {
        for(auto it_2 = it+1; it_2 != in.end();) {
            if(*it == *it_2) {
                it_2 = in.erase(it_2);
            } else {
                ++it_2;
            }
        }
    }
}

// The result of this function is a vector with all options expanded.
std::vector<std::string> expand_regex(std::string& regex_line, const int stack_level = 0) {
    //std::cout << "expand_regex: (" << regex_line << ") (" << stack_level << ")" << std::endl;
    // Record all options
    std::vector<std::vector<std::vector<std::string>>> options;
    options.push_back(std::vector<std::vector<std::string>>()); // initial empty vector.
    options.rbegin()->push_back(std::vector<std::string>());
    options.rbegin()->rbegin()->push_back("");
    while(regex_line.size() != 0) {
        //std::cout << "Main loop" << std::endl;
        //print_options(options, std::cout);
        if((regex_line[0] == '^')||(regex_line[0] == '$')) {
            //std::cout << "branch 1" << std::endl;
            // Skip these characters
            regex_line.erase(0,1);
        } else if(regex_line[0] == '(') {
            //std::cout << "branch 2" << std::endl;
            regex_line.erase(0,1);
            options.rbegin()->push_back(expand_regex(regex_line, stack_level+1));
            if(regex_line[0] != ')') {
                throw std::runtime_error("Didn't get expected end parens");
            }
            // consume end parens
            regex_line.erase(0,1);
            // Add new empty option.
            options.rbegin()->push_back(std::vector<std::string>());
            options.rbegin()->rbegin()->push_back("");
        } else if(regex_line[0] == '|') {
            //std::cout << "branch 3" << std::endl;
            // Start new option section
            options.push_back(std::vector<std::vector<std::string>>());
            options.rbegin()->push_back(std::vector<std::string>());
            options.rbegin()->rbegin()->push_back("");
            regex_line.erase(0,1);
        } else if(regex_line[0] == ')') {
            //std::cout << "branch 4" << std::endl;
            break; // break here. the superior call will finish
        } else {
            //std::cout << "branch 5" << std::endl;
            // Add these characters to the last group of the last option set.
            options.rbegin()->rbegin()->rbegin()->push_back(regex_line[0]); // Add to the last option.
            regex_line.erase(0,1);
        }
    }
    // Container for results
    std::vector<std::string> resulting_options;
    while(options.size() != 0) {
        // perform multiplexing of strings
        std::vector<std::string> option_list = combine(options[0]);
        //std::cout << "Combined option list:" << std::endl;
        //ConstForEach(option_list, [](const std::string& in) {
        //    std::cout << in << std::endl;
        //});
        // Add to resulting options.
        resulting_options.insert(resulting_options.end(), option_list.begin(), option_list.end());
        // Remove the element from options
        options.erase(options.begin());
    }
    remove_duplicates(resulting_options);
    return resulting_options;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    int test_value = 0;
    bool test_value_given = false;
    std::string test_expand_regex;
    bool test_expand_regex_given = false;
	ArgParse::ArgParser Parser("Task 39");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-t/--test-val", "Give a test value.", &test_value, ArgParse::Argument::Optional, &test_value_given);
    Parser.AddArgument("-ter", "Test expand regex given", &test_expand_regex, ArgParse::Argument::Optional, &test_expand_regex_given);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

    if(test_expand_regex_given) {
        std::cout << "Given regex: " << test_expand_regex << std::endl;
        ConstForEach(expand_regex(test_expand_regex), [](const std::string& path) {
            std::cout << path << std::endl;
        });
        return 0;
    }

	// Open input as stream
	std::ifstream infile(input_filepath);
    std::string line;

    // get regex
    std::string regex_line;
    std::getline(infile, regex_line);

    if(verbose) {
        std::cout << "Passed Regex: " << regex_line << std::endl;
    }

    // Expand the regex out.
    std::vector<std::string> unique_paths = expand_regex(regex_line);

    if(verbose) {
        std::cout << "Expanded regexs: " << std::endl;
        ConstForEach(unique_paths, [](const std::string& in) {
            std::cout << in << std::endl;
        });
    }

    // Get room extents
    int x_min = 0;
    int x_max = 0;
    int y_min = 0;
    int y_max = 0;

    for(size_t idx = 0; idx < unique_paths.size(); ++idx) {
        int x = 0;
        int y = 0;
        for(size_t c_idx = 0; c_idx < unique_paths[idx].size(); ++c_idx) {
            char the_char = unique_paths[idx][c_idx];
            // Move
            if(the_char == 'E') {
                x += 1;
            } else if (the_char == 'W') {
                x -= 1;
            } else if (the_char == 'N') {
                y -= 1;
            } else if (the_char == 'S') {
                y += 1;
            } else {
                throw std::runtime_error("Unknown character!");
            }
            // Update limits
            if(x < x_min) {
                x_min = x;
            }
            if(x > x_max) {
                x_max = x;
            }
            if(y < y_min) {
                y_min = y;
            }
            if(y > y_max) {
                y_max = y;
            }
        }
    }

    if(verbose) {
        std::cout << "Detected extents: x:[" << x_min << ", " << x_max << "] y:[" << y_min << ", " << y_max << "]" << std::endl;
    }

    // Build map
    p_idx n_rooms_x = x_max-x_min+1;
    p_idx n_rooms_y = y_max-y_min+1;
    array_2d<char> the_map(2*n_rooms_x+1,2*n_rooms_y+1);
    auto x_to_idx = [&](p_idx x) {
        return x-x_min;
    };
    auto y_to_idx = [&](p_idx y) {
        return y-y_min;
    };
    /*
    auto idx_to_x = [&](p_idx idx) {
        return idx+x_min;
    };
    auto idx_to_y = [&](p_idx idx) {
        return idx+y_min;
    };
    */

    // Initialize map.
    for(p_idx x_idx = 0; x_idx < 2*n_rooms_x+1; x_idx += 1) {
        for(p_idx y_idx = 0; y_idx < 2*n_rooms_y+1; y_idx += 1) {
            if((x_idx%2 == 0)&&(y_idx%2 == 0)) {
                the_map.assign(x_idx,y_idx) = '#';
            } else {
                the_map.assign(x_idx,y_idx) = '?';
            }
        }
    }
    // Assign initial room
    the_map.assign(1+2*x_to_idx(0),1+2*y_to_idx(0)) = '.';

    if(verbose) {
        std::cout << "Initial Map" << std::endl;
        the_map.print(std::cout);
    }

    // Traverse map with paths
    for(size_t path_idx = 0; path_idx < unique_paths.size(); ++path_idx) {
        std::string& path = unique_paths[path_idx];
        p_idx room_x = 0;
        p_idx room_y = 0;
        for(size_t idx = 0; idx < path.size(); ++idx) {
            char the_char = path[idx];
            //std::cout << "The char: " << the_char << std::endl;
            switch(the_char) {
                case ('N'): {
                    p_idx x_idx = 1+2*x_to_idx(room_x);
                    p_idx y_idx = 1+2*y_to_idx(room_y);
                    p_idx y_idx_next = 1+2*y_to_idx(room_y-1);
                    p_idx y_idx_door = y_idx-1;
                    char door_char = the_map(x_idx,y_idx_door);
                    char next_room_char = the_map(x_idx,y_idx_next);
                    if((next_room_char != '.')&&(next_room_char != '?')) {
                        std::cout << next_room_char << std::endl;
                        throw std::runtime_error("Unexpected room character!");
                    }
                    if((door_char != '-')&&(door_char != '?')) {
                        throw std::runtime_error("Unexpected door character!");
                    }
                    the_map.assign(x_idx, y_idx_door) = '-';
                    the_map.assign(x_idx, y_idx_next) = '.';
                    room_y -= 1;
                    break;
                }
                case ('S'): {
                    p_idx x_idx = 1+2*x_to_idx(room_x);
                    p_idx y_idx = 1+2*y_to_idx(room_y);
                    p_idx y_idx_next = 1+2*y_to_idx(room_y+1);
                    p_idx y_idx_door = y_idx+1;
                    char door_char = the_map(x_idx,y_idx_door);
                    char next_room_char = the_map(x_idx,y_idx_next);
                    if((next_room_char != '.')&&(next_room_char != '?')) {
                        std::cout << next_room_char << std::endl;
                        throw std::runtime_error("Unexpected room character!");
                    }
                    if((door_char != '-')&&(door_char != '?')) {
                        throw std::runtime_error("Unexpected door character!");
                    }
                    the_map.assign(x_idx, y_idx_door) = '-';
                    the_map.assign(x_idx, y_idx_next) = '.';
                    room_y += 1;
                    break;
                }
                case('E'): {
                    p_idx y_idx = 1+2*y_to_idx(room_y);
                    p_idx x_idx = 1+2*x_to_idx(room_x);
                    p_idx x_idx_next = 1+2*y_to_idx(room_x+1);
                    p_idx x_idx_door = x_idx+1;
                    char door_char = the_map(x_idx_door,y_idx);
                    char next_room_char = the_map(x_idx_next,y_idx);
                    if((next_room_char != '.')&&(next_room_char != '?')) {
                        std::cout << next_room_char << std::endl;
                        throw std::runtime_error("Unexpected room character!");
                    }
                    if((door_char != '|')&&(door_char != '?')) {
                        throw std::runtime_error("Unexpected door character!");
                    }
                    the_map.assign(x_idx_door, y_idx) = '|';
                    the_map.assign(x_idx_next, y_idx) = '.';
                    room_x += 1;
                    break;
                }
                case('W'): {
                    p_idx y_idx = 1+2*y_to_idx(room_y);
                    p_idx x_idx = 1+2*x_to_idx(room_x);
                    p_idx x_idx_next = 1+2*y_to_idx(room_x-1);
                    p_idx x_idx_door = x_idx-1;
                    char door_char = the_map(x_idx_door,y_idx);
                    char next_room_char = the_map(x_idx_next,y_idx);
                    if((next_room_char != '.')&&(next_room_char != '?')) {
                        std::cout << next_room_char << std::endl;
                        throw std::runtime_error("Unexpected room character!");
                    }
                    if((door_char != '|')&&(door_char != '?')) {
                        throw std::runtime_error("Unexpected door character!");
                    }
                    the_map.assign(x_idx_door, y_idx) = '|';
                    the_map.assign(x_idx_next, y_idx) = '.';
                    room_x -= 1;
                    break;
                }
                default: {
                    std::runtime_error("Unexpected path character!");
                }
            }
        }
    }

    // Set all remaining '?' as '#'.
    for(p_idx x_idx = 0; x_idx < 1+2*n_rooms_x; ++x_idx) {
        for(p_idx y_idx = 0; y_idx < 1+2*n_rooms_y; ++y_idx) {
            if (the_map(x_idx,y_idx) == '?') {
                the_map.assign(x_idx,y_idx) = '#';
            }
        }
    }

    if(verbose) {
        std::cout << "After path processing:" << std::endl;
        the_map.print(std::cout);
    }

    // Calculate distance to each room from starting node.
    typedef std::pair<p_idx,p_idx> point;    
    typedef std::pair<point,p_idx> cost_pair;
    std::map<point,p_idx> distances; // map to store shortest distances
    std::vector<cost_pair> openSet;
    openSet.push_back(cost_pair(point(0,0),0));
    while(openSet.size() != 0) {
        p_idx dist = openSet[0].second;
        point room = openSet[0].first;
        // Remove from open set.
        openSet.erase(openSet.begin());
        auto find_it = std::find_if(distances.cbegin(),distances.cend(), [&](const cost_pair& in) {
            if(in.first == room) {
                return true;
            } else {
                return false;
            }
        });
        if(find_it == distances.cend()) {
            // haven't encountered the room yet.
            distances[room] = dist;
        } else {
            if(dist < distances[room]) {
                distances[room] = dist;
            }
        }
        // Add adjacent rooms if they haven't already been encountered or have higher scores.
        auto process_next_room = [&](const point& next_room) {
            auto find_it = std::find_if(distances.cbegin(),distances.cend(), [&](const cost_pair& in) {
                if(in.first == next_room) {
                    return true;
                } else {
                    return false;
                }
            });
            if(find_it == distances.cend()) {
                openSet.push_back(cost_pair(next_room, dist+1));
            } else {
                if(dist+1 < find_it->second) {
                    // Add the room if its better than the previous
                    openSet.push_back(cost_pair(next_room, dist+1));
                }
            }
        };
        // North direction
        if(the_map(1+2*x_to_idx(room.first),2*y_to_idx(room.second)) == '-') {
            point next_room = point(room.first,room.second-1);
            process_next_room(next_room);
        }
        // South direction
        if(the_map(1+2*x_to_idx(room.first),2+2*y_to_idx(room.second)) == '-') {
            point next_room = point(room.first,room.second+1);
            process_next_room(next_room);
        }
        // East direction
        if(the_map(2+2*x_to_idx(room.first),1+2*y_to_idx(room.second)) == '|') {
            point next_room = point(room.first+1,room.second);
            process_next_room(next_room);
        }
        // West direction
        if(the_map(2*x_to_idx(room.first),1+2*y_to_idx(room.second)) == '|') {
            point next_room = point(room.first-1,room.second);
            process_next_room(next_room);
        }
    }

    // Find the furthest room
    p_idx max_dist = std::numeric_limits<p_idx>::min();
    for(auto map_it = distances.cbegin(); map_it != distances.cend(); ++map_it) {
        if(map_it->second > max_dist) {
            max_dist = map_it->second;
        }
    }

    std::cout << "The distance to the farthest room is: " << max_dist << std::endl;

    if(test_value_given) {
        array_2d<char> answer_map;
        std::vector<std::vector<char>> temp_answer_map;

        // Read in resulting map
        std::getline(infile, line);

        while(std::getline(infile,line)) {
            std::vector<char> char_line;
            for(size_t idx = 0; idx < line.size(); ++idx) {
                char_line.push_back(line[idx]);
            }
            temp_answer_map.push_back(char_line);
        }
        answer_map.init(temp_answer_map[0].size(), temp_answer_map.size());
        for(size_t j = 0; j < temp_answer_map.size(); ++j) {
            for(size_t i = 0; i < temp_answer_map[i].size(); ++i) {
                answer_map.assign(i,j) = temp_answer_map[j][i];
            }
        }

        if(verbose) {
            std::cout << "Expected map: " << std::endl;
            answer_map.print(std::cout);
        }

        // Test that developed map is the same as expected.
        bool match = true;
        for(p_idx x_idx = 0; x_idx < 1+2*n_rooms_x; ++x_idx) {
            for(p_idx y_idx = 0; y_idx < 1+2*n_rooms_y; ++y_idx) {
                char map_char = the_map(x_idx,y_idx);
                char expected_char = answer_map(x_idx,y_idx);
                if((x_idx == 1+2*x_to_idx(0))&&(y_idx == 1+2*y_to_idx(0))) {
                    map_char = 'X';
                }
                if(map_char != expected_char) {
                    match = false;
                    break;
                }
            }
        }
        if(!match) {
            std::cout << "Map wasn't a match.." << std::endl;
        } else {
            std::cout << "Map matched!" << std::endl;
        }
    }

	return 0;
}
