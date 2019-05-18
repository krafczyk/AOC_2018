#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>
#include <regex>
#include "ArgParseStandalone.h"
#include "utilities.h"

class Registers {
	public:
		typedef int Reg_t;
		Registers(Reg_t a, Reg_t b, Reg_t c, Reg_t d, Reg_t e, Reg_t f) {
			this->values[0] = a;
			this->values[1] = b;
			this->values[2] = c;
			this->values[3] = d;
			this->values[4] = e;
			this->values[5] = f;
		}
		Registers() : Registers(0,0,0,0,0,0) {
		}
		Registers(const Registers& rhs) {
            for(size_t idx = 0; idx < num_registers; ++idx) {
                this->values[idx] = rhs.values[idx];
            }
		}
		Registers& operator=(const Registers& rhs) {
            for(size_t idx = 0; idx < num_registers; ++idx) {
                this->values[idx] = rhs.values[idx];
            }
			return (*this);
		}
		Reg_t operator()(size_t i) const {
			return values[i];
		}
		Reg_t& assign(size_t i) {
			return values[i];
		}
		bool operator==(const Registers& rhs) const {
			for(size_t idx = 0; idx < num_registers; ++idx) {
				if(this->values[idx] != rhs.values[idx]) {
					return false;
				}
			}
			return true;
		}
        std::string get_str() const {
            std::stringstream ss;
            ss << "[";
            for(size_t idx = 0; idx < num_registers; ++idx) {
                ss << values[idx] << ", ";
            }
            ss << "]";
            return ss.str();
        }
        static const int num_registers = 6;
	private:
		// 6 stored values
		Reg_t values[num_registers];
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

struct instruction {
    std::string name;
    int A;
    int B;
    int C;
};

int run_program(int ip, Registers& reg, std::vector<instruction> program, int max_inst = -1, bool debug = false) {
    int n_inst = 0;
    if(debug) {
        std::cout << "Initial regs: " << reg.get_str() << std::endl;
    }
    while(((max_inst == -1)||(n_inst < max_inst))&&(reg(ip) < (Inst_t) program.size())) {
        n_inst += 1;
        // Fetch instruction:
        instruction& the_inst = program[reg(ip)];
        if(debug) {
            std::cout << n_inst << "| " << reg(ip) << ": ";
            std::cout << the_inst.name << " " << the_inst.A << " " << the_inst.B << " " << the_inst.C << std::endl;
        }
        // Execute the instruction
        instructions[the_inst.name](the_inst.A, the_inst.B, the_inst.C, reg);
        if(debug) {
            std::cout << reg.get_str() << std::endl;
        }
        // Increment instruction pointer
        reg.assign(ip) = reg(ip)+1;
    }
    return reg(0);
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
    int max_inst = -1;
    int reg_0 = 0;
	ArgParse::ArgParser Parser("Task 41");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
    Parser.AddArgument("-mi/--max-inst", "Maximum number of instructions to execute", &max_inst);
    Parser.AddArgument("-r", "Set initial value of register 0", &reg_0);
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

    std::regex ip_regex("#ip (\\d)", std::regex::ECMAScript);
    std::regex instruction_regex("(\\S\\S\\S\\S) (\\d+) (\\d+) (\\d+)", std::regex::ECMAScript);

    std::getline(infile, line);
    // Get the ip register.
    std::smatch ip_result;
    std::regex_match(line, ip_result, ip_regex);

    int ip = std::atoi(ip_result[1].str().c_str());

    std::cout << "IP: " << ip << std::endl;

    std::vector<instruction> program;
    while(std::getline(infile, line)) {
        std::smatch line_result;
        std::regex_search(line, line_result, instruction_regex);
        instruction new_instruction;
        new_instruction.name = line_result[1].str();
        new_instruction.A = std::atoi(line_result[2].str().c_str());
        new_instruction.B = std::atoi(line_result[3].str().c_str());
        new_instruction.C = std::atoi(line_result[4].str().c_str());
        program.push_back(new_instruction);
    }

    // Create registers.
    Registers reg;

    if(max_inst != -1) {
        run_program(ip, reg, program, max_inst, true);
    }

	return 0;
}
