CXX=g++
CXXFLAGS=-Wall -std=c++1z -g

TEST_FLAGS=-I. -L.
TEST_LIBS=gtest_main.o -lgtest -lboost_coroutine -lboost_system -lpthread

.PHONY: all clean tests check

all: tests libpyjamas.so

clean:
	rm json-parser.o lexer.o token.o libpyjamas.so test-json-lexer test-json-parser

tests: test-json-lexer test-json-parser

check: tests libpyjamas.so
	./test-json-lexer
	./test-json-parser

gtest_main.o: /usr/src/gtest/src/gtest_main.cc
	$(CXX) -c $< -o $@

libpyjamas.so: json-parser.o json-value.o lexer.o token.o
	$(CXX) $(CXXFLAGS) -fPIC -shared $+ -o $@

json-parser.o: json-parser.cpp json-parser.h lexer.h token.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

json-value.o: json-value.cpp json-value.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

lexer.o: lexer.cpp lexer.h token.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

token.o: token.cpp token.h
	$(CXX) $(CXXFLAGS) -fPIC -c $< -o $@

test-json-lexer: gtest_main.o test/test-json-lexer.cpp lexer.h token.h | libpyjamas.so
	$(CXX) $(CXXFLAGS) test/test-json-lexer.cpp -o $@ $(TEST_FLAGS) -lpyjamas $(TEST_LIBS)

test-json-parser: gtest_main.o test/test-json-parser.cpp json-value.h json-parser.h token.h | libpyjamas.so
	$(CXX) $(CXXFLAGS) test/test-json-parser.cpp -o $@ $(TEST_FLAGS) -lpyjamas $(TEST_LIBS)

