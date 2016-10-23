#include "json-parser.h"
#include <initializer_list>
#include <iostream>
#include <vector>

#include <gtest/gtest.h>

#include <boost/range/algorithm/equal.hpp>

using namespace pyjamas;

using TokenSequence = std::vector< Token >;
using boost::equal;

auto tokens( const std::initializer_list<Token>& toks ) -> std::vector<Token>
{
    return toks;
}

TEST(ut_json_lexer, parses_empty_input_and_all_whitespace )
{
    EXPECT_EQ( get_tokens( "" ),     tokens({ }) );
    EXPECT_EQ( get_tokens( " " ),    tokens({ }) );
    EXPECT_EQ( get_tokens( "\t" ),   tokens({ }) );
    EXPECT_EQ( get_tokens( "\r" ),   tokens({ }) );
    EXPECT_EQ( get_tokens( "\n" ),   tokens({ }) );
    EXPECT_EQ( get_tokens( "\r\n" ), tokens({ }) );
}

TEST( ut_json_lexer, leading_and_trailing_whitespace_is_ignored )
{
    EXPECT_EQ( get_tokens( " null " ), tokens({ Token::null }) );
}

TEST( ut_json_lexer, parses_null )
{
    EXPECT_EQ(
        get_tokens( "null" ),
        tokens( {
            Token::null } ));
}

TEST( ut_json_lexer, parses_true )
{
    EXPECT_EQ(
        get_tokens( "true" ),
        tokens( { true } ));
}

TEST( ut_json_lexer, parses_false )
{
    EXPECT_EQ(
        get_tokens( "false" ),
        tokens( { false } ));
}

TEST(ut_json_lexer,  returns_bad_token_for_some_parse_errors )
{
    auto ts = get_tokens( "moustache" );
    ASSERT_TRUE( ts.size() == 1 );
    ASSERT_TRUE( TokenType::Invalid == ts.front().type );
}

TEST( ut_json_lexer, parses_array_tokens )
{
    EXPECT_EQ(
        get_tokens( "[]" ),
        tokens( { Token::array_begin, Token::array_end }));

    EXPECT_EQ(
        get_tokens( "[ ]" ),
        tokens( { Token::array_begin, Token::array_end }));

    EXPECT_EQ(
        get_tokens( "[true, false]" ),
        tokens( {
            Token::array_begin,
            true,
            Token::item_separator,
            false,
            Token::array_end }));
}

TEST( ut_json_lexer, parses_strings )
{
    EXPECT_EQ(
        get_tokens( R"__("")__" ),
        tokens( { Token{ "" } }));

    EXPECT_EQ(
        get_tokens( R"__("blah")__" ),
        tokens( { Token{ "blah" } }));
}

TEST( ut_json_lexer, parses_strings_with_escape_sequences )
{
    // \" -> "
    EXPECT_EQ(
        get_tokens( R"_("\"")_" ),
        tokens( { "\"" }));

    EXPECT_EQ(
        get_tokens( R"_("\\")_" ),
        tokens( { R"_(\)_" }));

    EXPECT_EQ(
        get_tokens( R"_("\/")_" ),
        tokens( { R"_(/)_" }));

    EXPECT_EQ(
        get_tokens( R"_("\b")_" ),
        tokens( { "\b" }));

    EXPECT_EQ(
        get_tokens( R"_("\f")_" ),
        tokens( { "\f" }));

    EXPECT_EQ(
        get_tokens( R"_("\n")_" ),
        tokens( { "\n" }));

    EXPECT_EQ(
        get_tokens( R"_("\r")_" ),
        tokens( { "\r" }));

    EXPECT_EQ(
        get_tokens( R"_("\t")_" ),
        tokens( { "\t" }));
}

TEST( ut_json_lexer, invalid_escape_sequence_produces_invalid_token )
{
    auto toks = get_tokens( R"_("\%")_" );
    ASSERT_TRUE( toks.size() == 1 );
    ASSERT_TRUE( toks.begin()->type == TokenType::Invalid );
}

TEST( ut_json_lexer, parses_object_tokens )
{
    EXPECT_EQ(
        get_tokens( "{}" ),
        tokens( { Token::object_begin, Token::object_end }));

    EXPECT_EQ(
        get_tokens( "{ }" ),
        tokens( { Token::object_begin, Token::object_end }));

    EXPECT_EQ(
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
            Token::object_end }));
}
