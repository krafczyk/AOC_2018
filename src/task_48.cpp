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
        type effective_power() const {
            if(this->units < 0) {
                return 0;
            }
            return this->ap*this->units;
        }
        type damage(const group& target) const {
            if(hasElement(target.immunities,this->attack_type)) {
                // No damage, the target is immune!
                return 0;
            }
            if(hasElement(target.weaknesses, this->attack_type)) {
                // Double damage!
                return 2*this->effective_power();
            }
            return this->effective_power();
        }
        group() = default;
        group(const group& rhs) {
            this->units = rhs.units;
            this->hp = rhs.hp;
            this->ap = rhs.ap;
            this->initiative = rhs.initiative;
            this->attack_type = rhs.attack_type;
            this->weaknesses = rhs.weaknesses;
            this->immunities = rhs.immunities;
        }
        group& operator=(const group& rhs) {
            this->units = rhs.units;
            this->hp = rhs.hp;
            this->ap = rhs.ap;
            this->initiative = rhs.initiative;
            this->attack_type = rhs.attack_type;
            this->weaknesses = rhs.weaknesses;
            this->immunities = rhs.immunities;
            return *this;
        }
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
        ~army() {
            for(size_t i = 0; i < groups.size(); ++i) {
                delete groups[i];
            }
        }
        army() = default;
        army(const army& rhs) {
            for(size_t idx = 0; idx < rhs.groups.size(); ++idx) {
                groups.push_back(new group(*rhs.groups[idx]));
            }
        }
        std::vector<group*> groups;
        friend std::ostream& operator<<(std::ostream& out, army& ary);
};

std::ostream& operator<<(std::ostream& out, army& ary) {
    for(auto grp_it = ary.groups.begin(); grp_it != ary.groups.end(); ++grp_it) {
        out << *(*grp_it) << std::endl;
    }
    out << std::endl;
    return out;
}

void fight(army& A, army& B, bool verbose = false) {
/*
Immune System:
Group 1 contains 17 units
Group 2 contains 989 units
Infection:
Group 1 contains 801 units
Group 2 contains 4485 units

Infection group 1 would deal defending group 1 185832 damage
Infection group 1 would deal defending group 2 185832 damage
Infection group 2 would deal defending group 2 107640 damage
Immune System group 1 would deal defending group 1 76619 damage
Immune System group 1 would deal defending group 2 153238 damage
Immune System group 2 would deal defending group 1 24725 damage

Infection group 2 attacks defending group 2, killing 84 units
Immune System group 2 attacks defending group 1, killing 4 units
Immune System group 1 attacks defending group 2, killing 51 units
Infection group 1 attacks defending group 1, killing 17 units
*/
    if(verbose) {
        std::cout << "Army A:" << std::endl;
        for(size_t idx = 0; idx < A.groups.size(); ++idx) {
            std::cout << "Group " << (idx+1) << " contains " << A.groups[idx]->units << " units" << std::endl;
        }
        std::cout << "Army B:" << std::endl;
        for(size_t idx = 0; idx < B.groups.size(); ++idx) {
            std::cout << "Group " << (idx+1) << " contains " << B.groups[idx]->units << " units" << std::endl;
        }
        std::cout << std::endl;
    }

    // Target Selection
    army* army_selector[2] = {&A, &B};
    typedef std::pair<size_t,size_t> global_idx;
    std::vector<global_idx> groups;
    for(size_t idx = 0; idx != A.groups.size(); ++idx) {
        groups.push_back(global_idx(0,idx));
    }
    for(size_t idx = 0; idx != B.groups.size(); ++idx) {
        groups.push_back(global_idx(1,idx));
    }
    // Sort groups into order of highest effective power and initiative first.
    std::sort(groups.begin(), groups.end(), [&army_selector](const global_idx& idx_a, const global_idx& idx_b) {
        group* a = army_selector[idx_a.first]->groups[idx_a.second];
        group* b = army_selector[idx_b.first]->groups[idx_b.second];
        if(a->effective_power() > b->effective_power()) {
            return true;
        } else if (a->effective_power() == b->effective_power()) {
            if(a->initiative > b->initiative) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    });

    if(verbose) {
        std::cout << "priority list:" << std::endl;
        for(auto it = groups.begin(); it != groups.end(); ++it) {
            if(it->first == 0) {
                std::cout << "A group ";
            } else {
                std::cout << "B group ";
            }
            group* grp = army_selector[it->first]->groups[it->second];
            std::cout << (it->second+1) << " with effective power: " << grp->effective_power() << " and initiative " << grp->initiative << std::endl;
        }
        std::cout << std::endl;
    }

    std::map<group*,group*> targets;
    // Compute the attacks.

    for(auto it = groups.begin(); it != groups.end(); ++it) {
        size_t allies_idx = (it->first);
        size_t enemies_idx = (it->first+1)%2;
        army* allies = army_selector[allies_idx];
        army* enemies = army_selector[enemies_idx];
        group* attacker = allies->groups[it->second];
        std::vector<size_t> defender_idxs;
        for(size_t idx = 0; idx < enemies->groups.size(); ++idx) {
            bool valid = true;
            for(auto attack_it = targets.begin(); attack_it != targets.end(); ++attack_it) {
                if(attack_it->second == enemies->groups[idx]) {
                    valid = false;
                    break;
                }
            }
            if(valid) {
                defender_idxs.push_back(idx);
            }
        }
        // Sort the possible defenders by how attack strength, effective power, then initiative
        std::sort(defender_idxs.begin(), defender_idxs.end(), [&attacker,&enemies](const size_t a, const size_t b) {
            group* A = enemies->groups[a];
            group* B = enemies->groups[b];
            if(attacker->damage(*A) > attacker->damage(*B)) {
                return true;
            } else if (attacker->damage(*A) < attacker->damage(*B)) {
                return false;
            }
            if(A->effective_power() > B->effective_power()) {
                return true;
            } else if (A->effective_power() < B->effective_power()) {
                return false;
            }
            if(A->initiative > B->initiative) {
                return true;
            } else {
                return false;
            }
        });
        if(verbose) {
            for(size_t idx = 0; idx < defender_idxs.size(); ++idx) {
                if(allies_idx == 0) {
                    std::cout << "A group ";
                } else {
                    std::cout << "B group ";
                }
                std::cout << (it->second+1) << " would deal defending group " << (idx+1) << " " << attacker->damage(*(enemies->groups[idx])) << " damage" << std::endl;
            }
        }
        // Sort the possible defenders by how attack strength, effective power, then initiative
        if(defender_idxs.size() != 0) {
            if(verbose){
                std::cout << "* ";
                if(allies_idx == 0) {
                    std::cout << "A group ";
                } else {
                    std::cout << "B group ";
                }
                std::cout << (it->second+1) << " will attack ";
                if(allies_idx == 0) {
                    std::cout << "B group ";
                } else {
                    std::cout << "A group ";
                }
                std::cout << (defender_idxs[0]+1) << std::endl;
            }
            targets[attacker] = enemies->groups[defender_idxs[0]];
        }
    }
    if(verbose) {
        std::cout << std::endl;
    }

    // Attacking
    // Reorder groups by initiative, highest first.
    std::sort(groups.begin(), groups.end(), [&army_selector](const global_idx idx_a, const global_idx idx_b) {
        group* a = army_selector[idx_a.first]->groups[idx_a.second];
        group* b = army_selector[idx_b.first]->groups[idx_b.second];
        if(a->initiative > b->initiative) {
            return true;
        } else {
            return false;
        }
    });
    for(auto attack_it = groups.begin(); attack_it != groups.end(); ++attack_it) {
        group* attacker = army_selector[attack_it->first]->groups[attack_it->second];
        group* defender = targets[attacker];
        if(defender != nullptr) {
            type damage = attacker->damage(*defender);
            type units_killed = damage/defender->hp;
            defender->units -= units_killed;
        }
    }
    // Remove dead units
    auto remove_dead_units = [](std::vector<group*>& group_list) {
        for(auto it = group_list.begin(); it != group_list.end();) {
            if((*it)->units <= 0) {
                delete *it;
                it = group_list.erase(it);
            } else {
                ++it;
            }
        }
    };
    remove_dead_units(A.groups);
    remove_dead_units(B.groups);
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 48");
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
            group* new_group = new group();
            fetch_value(new_group->units, match_results[1].str());
            fetch_value(new_group->hp, match_results[2].str());
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
                                    new_group->immunities.push_back(match_string);
                                } else if (specialty_type == "weak") {
                                    new_group->weaknesses.push_back(match_string);
                                }
                            }
                        }
                    }
                }
            }
            fetch_value(new_group->ap, match_results[4].str());
            fetch_value(new_group->attack_type, match_results[5].str());
            fetch_value(new_group->initiative, match_results[6].str());
            current_army->groups.push_back(new_group);
        }
    }

    if(verbose) {
        std::cout << "Immune System" << std::endl;
        std::cout << immune;
        std::cout << "Infection" << std::endl;
        std::cout << infection;
    }

    while((immune.groups.size() != 0)&&(infection.groups.size() != 0)) {
        // Both sides still have units, fight!
        fight(immune, infection, verbose);
    }

    type total_units = 0;
    auto sum_units = [](std::vector<group*>& groups) {
        type total = 0;
        std::for_each(groups.begin(), groups.end(), [&total](const group* grp) {
            total += grp->units;
        });
        return total;
    };

    if(immune.groups.size() != 0) {
        total_units = sum_units(immune.groups);
    }
    if(infection.groups.size() != 0) {
        total_units = sum_units(infection.groups);
    }

    std::cout << "The winning side has " << total_units << " units" << std::endl;

	return 0;
}
