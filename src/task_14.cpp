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

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 13");
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

	// Read raw dependencies
	std::vector<std::pair<char, char>> raw_deps;
	std::string line;
	while(std::getline(infile, line)) {
		raw_deps.push_back(std::pair<char,char>(line[36], line[5]));
	}

	if (verbose) {
		std::cout << "Raw dependencies read in:" << std::endl;
		for(unsigned int idx = 0; idx < raw_deps.size(); ++idx) {
			std::cout << raw_deps[idx].first << " -> " << raw_deps[idx].second << std::endl;
		}
	}

	// Determine unique labels
	std::set<char> unique_labels;
	for(unsigned int idx = 0; idx < raw_deps.size(); ++idx) {
		auto the_pair = raw_deps[idx];
		unique_labels.insert(the_pair.first);
		unique_labels.insert(the_pair.second);
	}

	if (verbose) {
		std::cout << "Found the following unique labels:" << std::endl;
		for(auto uit = unique_labels.begin(); uit != unique_labels.end(); ++uit) {
			std::cout << *uit << std::endl;
		}
	}

	// Create a dependency data structure
	std::map<char, std::set<char>> dep_map;
	for(auto it = raw_deps.begin(); it != raw_deps.end(); ++it) {
		auto the_pair = *it;
		dep_map[the_pair.first].insert(the_pair.second);
	}

	// String to hold the answer
	std::string answer;
	// Current options to choose from
	std::set<char> options;
	// Current steps completed
	std::set<char> completed;

	// Add the labels with no deps to initial options
	for(auto uit = unique_labels.begin(); uit != unique_labels.end(); ++uit) {
		bool hasDeps = false;
		for (auto map_it = dep_map.begin(); map_it != dep_map.end(); ++map_it) {
			if (map_it->first == *uit) {
				hasDeps = true;
				break;
			}
		}
		if(!hasDeps) {
			options.insert(*uit);
		}
	}

	if (verbose) {
		std::cout << "Initial options are: ";
		for(auto op_it = options.begin(); op_it != options.end(); ++op_it) {
			std::cout << *op_it << std::endl;
		}
	}

	while(options.size() != 0) {
		// Pick first option
		char picked_option = *options.begin();
		// Add it to answer
		answer.push_back(picked_option);
		// Add it to completed steps
		completed.insert(picked_option);
		// Remove picked option from options
		options.erase(options.begin());
		// Add steps whose dependencies have been completed
		for(auto map_it = dep_map.begin(); map_it != dep_map.end(); ++map_it) {
			// The label we're checking for
			char check_label = map_it->first;
			// See if it's already an option
			if (hasElement(options.begin(), options.end(), check_label)) {
				continue;
			}
			// See if it's already completed
			if (hasElement(completed.begin(), completed.end(), check_label)) {
				continue;
			}
			// Okay, check if the dependencies are completed
			bool depsComplete = true;
			for(auto dep_it = map_it->second.begin(); dep_it != map_it->second.end(); ++dep_it) {
				if(!hasElement(completed.begin(), completed.end(), *dep_it)) {
					depsComplete = false;
					break;
				}
			}
			// Deps are complete, add to options!
			if (depsComplete) {
				options.insert(check_label);
			}
		}
	}

	// Check solution, are all steps completed?
	bool problem = false;
	for(auto uit = unique_labels.begin(); uit != unique_labels.end(); ++uit) {
		if (!hasElement(completed.begin(), completed.end(), *uit)) {
			std::cerr << "Error! the element " << *uit << " wasn't in the answer!!" << std::endl;
			problem = true;
		}
	}
	if(problem) {
		return -1;
	}

	std::cout << "The step order is: " << answer << std::endl;

	return 0;
}
