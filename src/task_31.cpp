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

std::vector<decltype(addr)*> instructions = {&addr, &addi, &mulr, &muli,
                                             &banr, &bani, &borr, &bori,
                                             &setr, &seti, &gtir, &gtri,
                                             &gtrr, &eqir, &eqri, &eqrr};

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
	ArgParse::ArgParser Parser("Task 31");
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
	while(std::getline(infile, line)) {
		if(state == 0) {
			Begin = create_registers(line.substr(8));
			state = 1;
		} else if (state == 1) {
			get_instruction(line, instruction);
			state = 2;
		} else if (state == 2) {
			After = create_registers(line.substr(8));
			state = 3;
			size_t num_matching = 0;
			std::for_each(instructions.cbegin(), instructions.cend(), [&](decltype(addr)* inst) {
				Registers temp = Begin;
				(*inst)(instruction[1], instruction[2], instruction[3], temp);
				if(After == temp) {
					num_matching += 1;
				}
			});
			std::cout << "The test matched " << num_matching << " instructions" << std::endl;
		} else if (state == 3) {
			// Skip
		} else {
			throw std::runtime_error("This shouldn't happen!");
		}
	}

	return 0;
}
