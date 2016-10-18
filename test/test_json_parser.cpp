#include "json_parser.h"
#include <initializer_list>
#include <iostream>
#include <vector>

#include "catch.hpp"

#include <boost/range/algorithm/equal.hpp>

using namespace pyjamas;

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

auto tokens( const std::initializer_list<Token>& toks ) -> std::vector<Token>
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
    REQUIRE_EQUAL( get_tokens( " null " ), tokens({ Token::null }) );
}

TEST_CASE( "Null", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "null" ),
        tokens( {
            Token::null } ));
}

TEST_CASE( "True", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "true" ),
        tokens( { true } ));
}

TEST_CASE( "False", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "false" ),
        tokens( { false } ));
}

TEST_CASE( "Bad token", "[json_parser]" )
{
    auto ts = get_tokens( "moustache" );
    REQUIRE( ts.size() == 1 );
    REQUIRE( TokenType::Invalid == ts.front().type );
}

TEST_CASE( "Arrays", "[json_parser]")
{
    REQUIRE_EQUAL(
        get_tokens( "[]" ),
        tokens( { Token::array_begin, Token::array_end }))

    REQUIRE_EQUAL(
        get_tokens( "[ ]" ),
        tokens( { Token::array_begin, Token::array_end }))

    REQUIRE_EQUAL(
        get_tokens( "[true, false]" ),
        tokens( {
            Token::array_begin,
            true,
            Token::item_separator,
            false,
            Token::array_end }))
}

TEST_CASE( "Strings", "[json_parser]")
{
    REQUIRE_EQUAL(
        get_tokens( R"__("")__" ),
        tokens( { Token{ "" } }));

    REQUIRE_EQUAL(
        get_tokens( R"__("blah")__" ),
        tokens( { Token{ "blah" } }));
}

TEST_CASE( "Strings with escape strings", "[json_parser]")
{
    // \" -> "
    REQUIRE_EQUAL(
        get_tokens( R"_("\"")_" ),
        tokens( { "\"" }));
}

TEST_CASE( "Invalid escape sequence doesn't crash", "[json_parser]")
{
    // TODO: bit of a shit test, make something reasonable
    get_tokens( R"_("\"")_" );
}

TEST_CASE( "Objects", "[json_parser]")
{
    REQUIRE_EQUAL(
        get_tokens( "{}" ),
        tokens( { Token::object_begin, Token::object_end }))

    REQUIRE_EQUAL(
        get_tokens( "{ }" ),
        tokens( { Token::object_begin, Token::object_end }))

    REQUIRE_EQUAL(
        get_tokens( "{\"a\": true, \"b\": false}" ),
        tokens( {
            Token::object_begin,
            "a",
            Token::key_value_separator,
            true,
            Token::item_separator,
            "b",
            Token::key_value_separator,
            false,
            Token::object_end }))
}
