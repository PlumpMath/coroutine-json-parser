#include "json-parser.h"
#include "lexer.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace pyjamas
{

// TODO; make this take a coroutine to give tokens to
// or a channel of some sort? signal slot? fiber?
auto get_tokens( std::istream& input ) -> std::vector< Token >
{
    auto tokens = std::vector< Token >{};

    Lexer lex( input, Lexer::OutChannelWriter{ [&](Lexer::OutChannelReader& reader) {
        while (reader)
        {
            auto token = reader.get();
            tokens.push_back( token );
            reader();
        }
    }} );

    lex.run();

    return tokens;
}

auto get_tokens( const char* string ) -> std::vector< Token >
{
    std::istringstream in{ string };
    return get_tokens( in );
}


}
