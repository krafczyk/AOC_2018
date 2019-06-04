CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++17 -fconcepts -O3 -Wall -Werror -Wextra -I utilities
#CXXFLAGS=-std=c++17 -fconcepts -g --coverage -Wall -Werror -Wextra -I utilities
#CXXFLAGS=-std=c++17 -fconcepts -g -pg -Wall -Werror -Wextra -I utilities

all: ./bin/task_1 ./bin/task_2 ./bin/task_3 ./bin/task_4 ./bin/task_5 ./bin/task_6 ./bin/task_7 ./bin/task_8 ./bin/task_9 ./bin/task_10 ./bin/task_11 ./bin/task_12 ./bin/task_13 ./bin/task_14 ./bin/task_15 ./bin/task_16 ./bin/task_17 ./bin/task_18 ./bin/task_19 ./bin/task_21 ./bin/task_22 ./bin/task_23 ./bin/task_24 ./bin/task_25 ./bin/task_26 ./bin/task_27 ./bin/task_28 ./bin/task_29 ./bin/task_30 ./bin/task_31 ./bin/task_32 ./bin/task_33 ./bin/task_34 ./bin/task_35 ./bin/task_36 ./bin/task_37 ./bin/task_38 ./bin/task_39 ./bin/task_41 ./bin/decompiled_task_41 ./bin/task_43 ./bin/task_44 ./bin/task_45 ./bin/task_46 ./bin/task_47 ./bin/task_48

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

./bin/task_36 : ./src/task_36.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -lncurses

clean:
	rm -rf ./bin/*
