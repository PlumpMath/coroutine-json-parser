#include "json_parser.h"
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

namespace pyjamas
{

std::ostream& operator<<( std::ostream& os, Token tok )
{
    switch (tok)
    {
        case Token::None:   return os << "None";
        case Token::End:    return os << "End";
        case Token::Null:   return os << "Null";
        case Token::True:   return os << "True";
        case Token::False:  return os << "False";
        default:            return os << "Invalid";
    }
}

JSONParser::JSONParser( std::istream& in ):
        in( in ),
        error( false ),
        token( Token::None )
{
    consume_char();
}

auto JSONParser::get_token() -> Token
{
    if (error)
        return Token::End;

    while (!error and std::isspace( c ))
        consume_char();

    if (error)
        return Token::End;

    return get_keyword();
}


auto JSONParser::get_keyword() -> Token
{
    auto get_string = [&] {
        std::string s;
        while (!error && std::isalpha( c ))
        {
            s += c;
            consume_char();
        }

        return s;
    };

    const auto& s = get_string();
    if (s == "null")
        return Token::Null;
    if (s == "true")
        return Token::True;
    if (s == "false")
        return Token::False;

    return Token::End;
}


void JSONParser::consume_char()
{
    assert( !error && "consume_char() should not be called when error == true" );

    in.get( c );
    if (!in)
        error = true;
}



// TODO; make this take a coroutine to give tokens to
// or a channel of some sort? signal slot? fiber?
auto get_tokens( const char* string ) -> std::vector< Token >
{
    std::istringstream input{ string };
    auto parser = JSONParser{ input };

    auto tokens = std::vector< Token >{};
    auto token = parser.get_token();
    while (token != Token::End)
    {
        tokens.push_back( token );
        token = parser.get_token();
    }

    return tokens;
}

}
