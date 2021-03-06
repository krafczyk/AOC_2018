#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include "ArgParseStandalone.h"
#include "utilities.h"

class Registers {
	public:
		typedef int Reg_t;
		Registers(Reg_t a, Reg_t b, Reg_t c, Reg_t d) {
			this->values[0] = a;
			this->values[1] = b;
			this->values[2] = c;
			this->values[3] = d;
		}
		Registers() : Registers(0,0,0,0) {
		}
		Registers(const Registers& rhs) {
			this->values[0] = rhs.values[0];
			this->values[1] = rhs.values[1];
			this->values[2] = rhs.values[2];
			this->values[3] = rhs.values[3];
		}
		Registers& operator=(const Registers& rhs) {
			this->values[0] = rhs.values[0];
			this->values[1] = rhs.values[1];
			this->values[2] = rhs.values[2];
			this->values[3] = rhs.values[3];
			return (*this);
		}
		Reg_t operator()(size_t i) const {
			return values[i];
		}
		Reg_t& assign(size_t i) {
			return values[i];
		}
		bool operator==(const Registers& rhs) const {
			for(size_t idx = 0; idx < 4; ++idx) {
				if(this->values[idx] != rhs.values[idx]) {
					return false;
				}
			}
			return true;
		}
	private:
		// 4 stored values
		Reg_t values[4];
};

typedef int Inst_t;

void addr(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)+in(B);
};

void addi(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)+B;
};

void mulr(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)*in(B);
};

void muli(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)*B;
};

void banr(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)&in(B);
};

void bani(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)&B;
};

void borr(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)|in(B);
};

void bori(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	in.assign(C) = in(A)|B;
};

void setr(Inst_t A, Inst_t B [[maybe_unused]], Inst_t C, Registers& in) {
	in.assign(C) = in(A);
};

void seti(Inst_t A, Inst_t B [[maybe_unused]], Inst_t C, Registers& in) {
	in.assign(C) = A;
};

void gtir(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	if(A > in(B)) {
		in.assign(C) = 1;
	} else {
		in.assign(C) = 0;
	}
};

void gtri(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	if(in(A) > B) {
		in.assign(C) = 1;
	} else {
		in.assign(C) = 0;
	}
};

void gtrr(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	if(in(A) > in(B)) {
		in.assign(C) = 1;
	} else {
		in.assign(C) = 0;
	}
};

void eqir(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	if(A == in(B)) {
		in.assign(C) = 1;
	} else {
		in.assign(C) = 0;
	}
};

void eqri(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	if(in(A) == B) {
		in.assign(C) = 1;
	} else {
		in.assign(C) = 0;
	}
};

void eqrr(Inst_t A, Inst_t B, Inst_t C, Registers& in) {
	if(in(A) == in(B)) {
		in.assign(C) = 1;
	} else {
		in.assign(C) = 0;
	}
};

std::map<std::string,decltype(addr)*> instructions =
 {{"addr", &addr}, {"addi", &addi}, {"mulr", &mulr}, {"muli", &muli},
  {"banr", &banr}, {"bani", &bani}, {"borr", &borr}, {"bori", &bori},
  {"setr", &setr}, {"seti", &seti}, {"gtir", &gtir}, {"gtri", &gtri},
  {"gtrr", &gtrr}, {"eqir", &eqir}, {"eqri", &eqri}, {"eqrr", &eqrr}};

Registers create_registers(std::string in) {
	std::string inside = in.substr(1,in.size()-2);
	size_t idx = 0;
	std::stringstream ss(inside);
	std::string temp;
	Registers answer;
	while(std::getline(ss, temp, ',')) {
		answer.assign(idx) = std::atoi(temp.c_str());
		idx += 1;
	}
	return answer;
}

void get_instruction(std::string in, Inst_t instruction[4]) {
	std::stringstream ss(in);
	std::string temp;
	size_t idx = 0;
	while(std::getline(ss, temp, ' ')) {
		instruction[idx] = std::atoi(temp.c_str());
		idx += 1;
	}
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 32");
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

	std::string line;
	Registers Begin;
	Registers After;
	Inst_t instruction[4];
	size_t state = 0; // State 0 is looking for Begin.
	// Initialize static map.
	std::map<std::string,std::vector<size_t>> instruction_statistics {
		{"addr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"addi", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"mulr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"muli", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"banr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"bani", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"borr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"bori", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"setr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"seti", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"gtir", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"gtri", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"gtrr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"eqir", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"eqri", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		{"eqrr", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
	};
	size_t num_lines_read = 0;
	while(std::getline(infile, line)) {
		num_lines_read += 1;
		if(state == 0) {
			if(line == "") {
				// Reached end of first part.
				break;
			}
			Begin = create_registers(line.substr(8));
			state = 1;
		} else if (state == 1) {
			get_instruction(line, instruction);
			state = 2;
		} else if (state == 2) {
			After = create_registers(line.substr(8));
			state = 3;
			ConstForEach(instructions, [&](const std::pair<std::string,decltype(addr)*>& item) {
				Registers temp = Begin;
				(*(item.second))(instruction[1], instruction[2], instruction[3], temp);
				if(After == temp) {
					instruction_statistics[item.first][instruction[0]] += 1;
				}
			});
		} else if (state == 3) {
			state = 0;
			// Skip
		} else {
			throw std::runtime_error("This shouldn't happen!");
		}
	}

	if(verbose) {
	std::cout << "Statistics collected: " << std::endl;
	ConstForEach(instruction_statistics, [&](const std::pair<std::string,std::vector<size_t>>& in) {
		std::cout << in.first << ": ";
		ConstForEach(in.second, [&](const size_t& a) {
				std::cout << std::setfill(' ') << std::setw(3) << a;
		});
		std::cout << std::endl;
	});
	}

	std::set<Inst_t> opcode_assigned;
	std::set<std::string> inst_assigned;
	std::map<Inst_t, std::string> opcode_map;

	while(opcode_map.size() != 16) {
		// Find the instruction which only match one opcode
		for(auto it = instruction_statistics.cbegin(); it != instruction_statistics.cend(); ++it) {
			if(hasElement(inst_assigned, it->first)) {
				continue;
			}
			size_t num_matches = 0;
			Inst_t last_match = 0;
			for(Inst_t opcode = 0;opcode < 16; ++opcode) {
				if(hasElement(opcode_assigned, opcode)) {
					continue;
				}
				if(it->second[opcode] != 0) {
					num_matches += 1;
					last_match = opcode;
				}
			}
			if(num_matches == 1) {
				// Found a match.
				opcode_map[last_match] = it->first;
				inst_assigned.insert(it->first);
				opcode_assigned.insert(last_match);
			}
		}
	}

	//Test that no opcode collides with any other, and opcode range
	//Show mapping as well.
	std::cout << "The opcode mapping is:" << std::endl;
	std::set<std::string> inst_names;
	ConstForEach(opcode_map, [&](const std::pair<Inst_t, std::string>& in) {
		std::cout << in.first << " -> " << in.second << std::endl;
		if(hasElement(inst_names, in.second)) {
			throw std::runtime_error("There was an opcode degeneracy!");
		} else {
			inst_names.insert(in.second);
		}
		if(in.first < 0) {
			throw std::runtime_error("Invalid Opcode! Negative!");
		}
		if(in.first >= 16) {
			throw std::runtime_error("Invalid Opcode! Too large!");
		}
	});

	// Run program!
	Registers reg;
	while(std::getline(infile, line)) {
		if(line == "") {
			continue;
		}
		// Get the instruction
		get_instruction(line, instruction);
		// run instruction
		instructions[opcode_map[instruction[0]]](instruction[1], instruction[2], instruction[3], reg);
	}

	std::cout << "Program result is: " << reg(0) << std::endl;

	return 0;
}
