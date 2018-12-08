CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++11 -O3 -Wall -Werror -Wextra -I utilities

all: ./bin/task_1 ./bin/task_2 ./bin/task_3 ./bin/task_4 ./bin/task_5 ./bin/task_6 ./bin/task_7 ./bin/task_8 ./bin/task_9 ./bin/task_10

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf ./bin/*
