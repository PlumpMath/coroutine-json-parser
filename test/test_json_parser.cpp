#include "json_parser.h"
#include <initializer_list>
#include <iostream>

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
    REQUIRE_EQUAL( get_tokens( " null " ), tokens({ TokenType::Null }) );
}

TEST_CASE( "Null", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "null" ),
        tokens( {
            TokenType::Null } ));
}

TEST_CASE( "True", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "true" ),
        tokens( { TokenType::True } ));
}

TEST_CASE( "False", "[json_parser]" )
{
    REQUIRE_EQUAL(
        get_tokens( "false" ),
        tokens( { TokenType::False } ));
}

TEST_CASE( "Bad token", "[json_parser]" )
{
    // TODO: doesn't seem ideal; probably want to see a parse error here rather than nothing.
    REQUIRE_EQUAL(
        get_tokens( "moustache" ),
        tokens( { TokenType::Invalid } ));
}
