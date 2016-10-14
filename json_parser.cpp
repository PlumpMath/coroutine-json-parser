#include <cassert>
#include <cctype>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/range/algorithm/equal.hpp>

enum class Token
{
    None,
    End,
    Null,
    True,
    False
};

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
struct JSONParser
{
    JSONParser( std::istream& in ):
            in( in ),
            error( false ),
            token( Token::None )
    {
        consume_char();
    }

    auto get_token() -> Token
    {
        if (error)
            return Token::End;

        while (!error and std::isspace( c ))
            consume_char();

        if (error)
            return Token::End;

        return get_keyword();
    }


    auto get_keyword() -> Token
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


    void consume_char()
    {
        assert( !error && "consume_char() should not be called when error == true" );

        in.get( c );
        if (!in)
            error = true;
    }
private:
    std::istream&  in;
    char           c;
    bool           error;
    Token          token; //< Result
};


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


// -----------------------------------------------------------------------------
//

#include "catch.hpp"

namespace catch_aux
{
    #define PRINT( a )                       \
    {                                        \
        std::cerr << #a << " {\n";           \
        for (const auto& v : a)              \
            std::cerr << "\t" << v << "\n";  \
        std::cerr << "}\n";                  \
    }

    #define REQUIRE_EQUAL( actual_, expected_ )    \
    {                                              \
        auto&& actual = actual_;                   \
        auto&& expected = expected_;               \
        if ( !equal( actual, expected ))           \
        {                                          \
            std::cerr << "REQUIRE_EQUAL\n";        \
            PRINT(expected);                       \
            PRINT(actual);                         \
            FAIL("FAILED");                        \
        }                                          \
    }
}

using namespace catch_aux;

using TokenSequence = std::vector< Token >;
using boost::equal;

auto tokens( const std::initializer_list<Token>& toks ) -> const std::initializer_list<Token>&
{
    return toks;
}

TEST_CASE( "No tokens", "[json_parser]" )
{
    REQUIRE_EQUAL( get_tokens( "" ),     tokens({ }) );
    REQUIRE_EQUAL( get_tokens( " " ),    tokens({ }) );
    REQUIRE_EQUAL( get_tokens( "\t" ),   tokens({ }) );
    REQUIRE_EQUAL( get_tokens( "\r" ),   tokens({ }) );
    REQUIRE_EQUAL( get_tokens( "\n" ),   tokens({ }) );
    REQUIRE_EQUAL( get_tokens( "\r\n" ), tokens({ }) );
}

TEST_CASE( "Leading and trailing whitespace is ignored", "[json_parser]" )
{
    REQUIRE_EQUAL( get_tokens( " null " ), tokens({ Token::Null }) );
}

TEST_CASE( "Null", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "null" ),
        tokens( {
            Token::Null } ));
}
