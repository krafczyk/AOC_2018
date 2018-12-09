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
	int num_workers = 5;
	int base_cost = 60;
	ArgParse::ArgParser Parser("Task 14");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);
	Parser.AddArgument("-n/--num-workers", "Number of workers", &num_workers, ArgParse::Argument::Optional);
	Parser.AddArgument("-b/--base-cost", "Base cost per step", &base_cost, ArgParse::Argument::Optional);

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
	// Current steps in progress
	std::map<int,std::pair<char,int>> workers;

	// Add idle workers
	for(int i=0; i<num_workers; ++i) {
		workers[i] = std::pair<char,int>(-1,-1);
	}

	auto is_idle __attribute__((unused)) = [](std::pair<char,int>& in) {
		if(in.first == -1) {
			return true;
		} else {
			return false;
		}
	};

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

	auto get_idle_worker_ids = [&workers,&is_idle]() {
		std::vector<int> idle_workers;
		for(auto worker_it = workers.begin(); worker_it != workers.end(); ++worker_it) {
			if(is_idle(worker_it->second)) {
				idle_workers.push_back(worker_it->first);
			}
		}
		return idle_workers;
	};

	auto get_job_cost = [base_cost](char job) {
		return base_cost + (((int)job)-64);
	};

	int time_spent = 0;
	std::vector<int> idle_workers;
	while(completed.size() != unique_labels.size()) {
		// doll out new jobs
		while((options.size() != 0)&&((idle_workers = get_idle_worker_ids()).size() != 0)) {
			// Pick first option
			char picked_option = *options.begin();
			// Remove picked option from options
			options.erase(options.begin());
			// Assign to idle worker
			int worker_id = idle_workers[0];
			workers[worker_id] = std::pair<char,int>(picked_option, get_job_cost(picked_option));
		}
		// Find the cheapest job
		int cheapest_job_cost = std::numeric_limits<int>::max();
		for(auto worker_it = workers.begin(); worker_it != workers.end(); ++worker_it) {
			if(!is_idle(worker_it->second)) {
				if(worker_it->second.second < cheapest_job_cost) {
					cheapest_job_cost = worker_it->second.second;
				}
			}
		}
		// Advance by cheapest cost
		for(auto worker_it = workers.begin(); worker_it != workers.end(); ++worker_it) {
			if(!is_idle(worker_it->second)) {
				worker_it->second.second -= cheapest_job_cost;
			}
		}
		// Advance time spent by cheapest cost
		time_spent += cheapest_job_cost;
		// Handle workers who have completed their jobs.
		// At least one worker should have completed their job.
		bool workerFinished = false;
		for(auto worker_it = workers.begin(); worker_it != workers.end(); ++worker_it) {
			if(!is_idle(worker_it->second)) {
				// job finished
				if(worker_it->second.second == 0) {
					// Add job to completed lit
					completed.insert(worker_it->second.first);
					// Add job to completed string
					answer.append(1, worker_it->second.first);
					// Indicate that a worker finished this round.
					workerFinished = true;
					// Make worker idle.
					worker_it->second.first = -1;
					worker_it->second.second = -1;
				}
			}
		}
		// Abort if no workder finished
		if(!workerFinished) {
			std::cerr << "No worker finished this round!!" << std::endl;
			return -1;
		}

		// Add steps whose dependencies have been completed
		for(auto map_it = dep_map.begin(); map_it != dep_map.end(); ++map_it) {
			// The label we're checking for
			char check_label = map_it->first;
			// See if it's already an option
			if (hasElement(options.begin(), options.end(), check_label)) {
				continue;
			}
			// See if its in progress
			bool in_progress = false;
			for(auto worker_it = workers.begin(); worker_it != workers.end(); ++worker_it) {
				if(worker_it->second.first == check_label) {
					in_progress = true;
					break;
				}
			}
			if(in_progress) {
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

	std::cout << "The time taken is: " << time_spent << std::endl;

	return 0;
}
