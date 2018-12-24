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
    //std::cout << "expand_regex (" << stack_level << "): " << regex_line << std::endl;
    //std::cout << "expand_regex: " << regex_line.size() << std::endl;
    std::vector<std::string> resulting_paths;
    std::vector<std::string> options;
    options.push_back(""); // initial empty string.
    int loop_it = 0;
    while((regex_line.size() != 0)&&(loop_it < 8)) {
        //std::cout << "main loop" << std::endl;
        if((regex_line[0] == '^')||(regex_line[0] == '$')) {
            //std::cout << "branch 1" << std::endl;
            regex_line.erase(0,1);
        } else if(regex_line[0] == '(') {
            //std::cout << "branch 2" << std::endl;
            regex_line.erase(0,1);
            combine(resulting_paths, options); // save current options.
            options.clear(); // clear current options.
            int loop_it_2 = 0;
            bool was_pipe = false;
            do {
                was_pipe = false;
                //std::cout << "miniloop" << std::endl;
                // Expand the current regex.
                //std::cout << "before expand: " << regex_line << std::endl;
                std::vector<std::string> sub_options = expand_regex(regex_line, stack_level +1);
                //std::cout << "after expand: " << regex_line << std::endl;
                //std::cout << "received sub_options length: " << sub_options.size() << std::endl;
                //ConstForEach(sub_options, [&](const std::string& in) {
                //        std::cout << "[" << in << "]" << std::endl;
                //});
                // Add to options
                options.insert(options.end(), sub_options.begin(), sub_options.end());
                if(regex_line[0] == '|') {
                    // Erase character.
                    regex_line.erase(0,1);
                    was_pipe = true;
                }
                //std::cout << "Character: " << regex_line[0] << std::endl;
                loop_it_2 += 1;
                if(loop_it_2 > 8) {
                    break;
                }
                //std::cout << "miniloop end" << std::endl;
            } while((regex_line[0] != ')')||(was_pipe));
            // Eat closing parentheses
            regex_line.erase(0,1);
            break;
        } else if((regex_line[0] == '|')||(regex_line[0] == ')')) {
            //std::cout << "branch 3" << std::endl;
            break;
            // If we didn't see a ')' first, then we should simply return.
        } else {
            //std::cout << "branch 4" << std::endl;
            options[0].push_back(regex_line[0]);
            regex_line.erase(0,1);
        }
        loop_it += 1;
    }
    //std::cout << "Ending expand_regex call." << std::endl;
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

    std::cout << "expanded paths are:" << std::endl;
    ConstForEach(expand_regex(regex_line), [](const std::string& path) {
        std::cout << path << std::endl;
    });

    std::vector<std::vector<char>> answer_map;
    if(test_value_given) {
        // Read in resulting map
        std::getline(infile, line);

        while(std::getline(infile,line)) {
            std::vector<char> char_line;
            for(size_t idx = 0; idx < line.size(); ++idx) {
                char_line.push_back(line[idx]);
            }
            answer_map.push_back(char_line);
        }
        if(verbose) {
            std::cout << "Expected map: " << std::endl;
            ConstForEach(answer_map, [](const std::vector<char>& char_line) {
                    ConstForEach(char_line, [](char c) {
                            std::cout << c;
                    });
                    std::cout << std::endl;
            });
        }
    }

	return 0;
}
