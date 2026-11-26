CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=build/%.o)

minic: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp src/*.hpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

test: minic
	bash tests/run_tests.sh

clean:
	rm -rf build minic

.PHONY: test clean
