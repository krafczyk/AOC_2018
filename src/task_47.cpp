#include <iostream>
#include <regex>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

typedef int type;

class group {
    public:
        type units;
        type hp;
        type ap;
        type initiative;
        std::string attack_type;
        std::vector<std::string> weaknesses;
        std::vector<std::string> immunities;
        friend std::ostream& operator<<(std::ostream& out, const group& grp);
};

std::ostream& operator<<(std::ostream& out, const group& grp) {
    //4547 units each with 21147 hit points (weak to fire; immune to radiation) with an attack that does 7 slashing damage at initiative 4
    out << grp.units << " units each with " << grp.hp;
    out << " hit points ";
    if((grp.weaknesses.size() > 0)||(grp.immunities.size() > 0)) {
        out << "(";
        size_t num = 0;
        if(grp.weaknesses.size() > 0) {
            num += 1;
            out << "weak to ";
            bool first = true;
            for(size_t i = 0; i < grp.weaknesses.size(); ++i) {
                if(first) {
                    first = false;
                    out << grp.weaknesses[i];
                } else {
                    out << ", " << grp.weaknesses[i];
                }
            }
        }
        if(grp.immunities.size() > 0) {
            if(num == 1) {
                out << "; ";
            }
            out << "immune to ";
            bool first = true;
            for(size_t i = 0; i < grp.immunities.size(); ++i) {
                if(first) {
                    first = false;
                    out << grp.immunities[i];
                } else {
                    out << ", " << grp.immunities[i];
                }
            }
        }
        out << ") ";
    }
    out << "with an attack that does " << grp.ap;
    out << " " << grp.attack_type << " damage at initiative " << grp.initiative;
    return out;
}

class army {
    public:
        std::vector<group> groups;
};

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 47");
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

    // Immune system.
    army immune;
    // Infection.
    army infection;

    std::string line;
    std::regex group_matcher("^(\\d+) units each with (\\d+) hit points (?:\\((.*)\\) )?with an attack that does (\\d+) (.*) damage at initiative (\\d+)$", std::regex::ECMAScript);
    std::regex specialties_matcher("^(?:(immune|weak) to (\\w*)(?:, (\\w*))*)(?:; (immune|weak) to (\\w*)(?:, (\\w*))*)*$", std::regex::ECMAScript);
    army* current_army = &immune;
    std::getline(infile, line);
    auto fetch_value = [](auto& val, std::string str) {
        std::istringstream iss(str);
        iss >> val;
    };
    while(std::getline(infile, line)) {
        std::smatch match_results;
        if(!std::regex_match(line, match_results, group_matcher)) {
            current_army = &infection;
        } else {
            group new_group;
            fetch_value(new_group.units, match_results[1].str());
            fetch_value(new_group.hp, match_results[2].str());
            std::string specialties = match_results[3].str();
            if(specialties.size() != 0) {
                std::smatch specialties_match;
                if(!std::regex_match(specialties, specialties_match, specialties_matcher)) {
                    std::cerr << "We had a problem matching specialties" << std::endl;
                    throw;
                } else {
                    std::string specialty_type;
                    for(size_t i = 1; i < specialties_match.size(); ++i) {
                        std::string match_string = specialties_match[i].str();
                        if((match_string == "immune")||(match_string == "weak")) {
                            specialty_type = match_string;
                        } else {
                            if(match_string.size() > 0) {
                                if(specialty_type == "immune") {
                                    new_group.immunities.push_back(match_string);
                                } else if (specialty_type == "weak") {
                                    new_group.weaknesses.push_back(match_string);
                                }
                            }
                        }
                    }
                }
            }
            fetch_value(new_group.ap, match_results[4].str());
            fetch_value(new_group.attack_type, match_results[5].str());
            fetch_value(new_group.initiative, match_results[6].str());
            current_army->groups.push_back(new_group);
        }
    }

    if(verbose) {
        std::cout << "Immune System" << std::endl;
        for(size_t i = 0; i < immune.groups.size(); ++i) {
            std::cout << immune.groups[i] << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Infection" << std::endl;
        for(size_t i = 0; i < infection.groups.size(); ++i) {
            std::cout << infection.groups[i] << std::endl;
        }
    }
	return 0;
}
