CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++11 -O3 -Wall -Werror -Wextra -I utilities

all: ./bin/task_1 ./bin/task_2

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf ./bin/*
