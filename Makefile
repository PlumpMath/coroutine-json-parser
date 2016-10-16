CXX=g++
CXXFLAGS=-Wall -std=c++1z -g

TEST_FLAGS=-I. -L.
TEST_LIBS=catch_main.o -lboost_coroutine -lboost_system

.PHONY: all clean tests check

all: tests libpyjamas.so

clean:
	rm json-parser.o libpyjamas.so test-json-parser

tests: test-json-parser

check: tests
	./test-json-parser

libpyjamas.so: json-parser.o
	$(CXX) $(CXXFLAGS) -fPIC -shared $+ -o $@

json-parser.o: json_parser.cpp json_parser.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

test-json-parser: test/test_json_parser.cpp json_parser.h
	$(CXX) $(CXXFLAGS) $< -o $@ $(TEST_FLAGS) -lpyjamas $(TEST_LIBS)

