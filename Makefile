CXX=g++
CXXFLAGS=-Wall -std=c++1z -g

TEST_FLAGS=-I. -L.
TEST_LIBS=gtest_main.o -lgtest -lboost_coroutine -lboost_system -lpthread

.PHONY: all clean tests check

all: tests libpyjamas.so

clean:
	rm json-parser.o libpyjamas.so test-json-parser

tests: test-json-parser

check: tests
	./test-json-parser

gtest_main.o: /usr/src/gtest/src/gtest_main.cc
	$(CXX) -c $< -o $@

libpyjamas.so: json-parser.o
	$(CXX) $(CXXFLAGS) -fPIC -shared $+ -o $@

json-parser.o: json_parser.cpp json_parser.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

test-json-parser: gtest_main.o test/test_json_parser.cpp json_parser.h
	$(CXX) $(CXXFLAGS) test/test_json_parser.cpp -o $@ $(TEST_FLAGS) -lpyjamas $(TEST_LIBS)

