CXX=g++
CXXFLAGS=-Wall -std=c++1z -g

TEST_FLAGS=-I.
TEST_LIBS=catch_main.o -lboost_coroutine -lboost_system

.PHONY: all tests check

all: tests

tests: test-json-parser

check: tests
	./test-json-parser

json-parser.o: json_parser.cpp json_parser.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

test-json-parser:  json-parser.o test/test_json_parser.cpp
	$(CXX) $(CXXFLAGS) $+ -o $@ $(TEST_FLAGS) $(TEST_LIBS)

