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

void combine(std::vector<std::string>& A, std::vector<std::string>& B) {
    if(A.size() == 0) {
        std::swap(A, B);
    } else {
        std::vector<std::string> new_paths;
        for(size_t p_idx = 0; p_idx < A.size(); ++p_idx) {
            for(size_t o_idx=0; o_idx < B.size(); ++o_idx) {
                new_paths.push_back(A[p_idx]+B[o_idx]);
            }
        }
        std::swap(A, new_paths);
    }
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
    std::vector<std::string> resulting_paths;
    std::vector<std::string> options;
    options.push_back(""); // initial empty string.
    while(regex_line.size() != 0) {
        if((regex_line[0] == '^')||(regex_line[0] == '$')) {
            regex_line.erase(0,1);
        } else if(regex_line[0] == '(') {
            regex_line.erase(0,1);
            combine(resulting_paths, options); // save current options.
            options.clear(); // clear current options.
            options = expand_regex(regex_line, stack_level+1);
            if(regex_line[0] != ')') {
                throw std::runtime_error("Didn't get expected end parens");
            }
            // consume end parens
            regex_line.erase(0,1);
            combine(resulting_paths, options); // save current options.
            options.clear(); // clear current options.
            options.push_back(""); // Add new empty option.
        } else if(regex_line[0] == '|') {
            // Add new option
            options.push_back("");
            regex_line.erase(0,1);
        } else if(regex_line[0] == ')') {
            combine(resulting_paths, options); // save current options.
            options.clear(); // clear current options.
            // Add new empty option
            options.push_back("");
            break; // break here. the superior call will finish
        } else {
            options[options.size()-1].push_back(regex_line[0]); // Add to the last option.
            regex_line.erase(0,1);
        }
    }
    combine(resulting_paths, options);
    remove_duplicates(resulting_paths);
    return resulting_paths;
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
                y += 1;
            } else if (the_char == 'S') {
                y -= 1;
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
    }

	return 0;
}
