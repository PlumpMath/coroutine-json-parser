#include "json-parser.h"

#include <gtest/gtest.h>
#include <boost/optional/optional_io.hpp>

using namespace pyjamas;

TEST(ut_json_parser, bad_token )
{
    EXPECT_EQ( boost::none, parse_json( "numpty" ) );
}

TEST(ut_json_parser, null )
{
    EXPECT_EQ( JsonValue( null{} ), parse_json( "null" ) );
}

TEST(ut_json_parser, array )
{
    EXPECT_EQ( JsonValue( array{} ), parse_json( "[]"  ) );

    EXPECT_EQ(
        JsonValue( array{ {
            JsonValue( null{} )
        }} ),
        parse_json( "[null]" ) );

    EXPECT_EQ(
        JsonValue( array{ {
            JsonValue( null{} ),
            JsonValue( boolean{ false } ),
            JsonValue( boolean{ true  } ),
        }} ),
        parse_json( "[null, false, true]" ) );}

TEST(ut_json_parser, bad_array )
{
    EXPECT_EQ( boost::none, parse_json( "[,]" ) );
    EXPECT_EQ( boost::none, parse_json( "[null,]" ) );
    EXPECT_EQ( boost::none, parse_json( "[,null]" ) );
    EXPECT_EQ( boost::none, parse_json( "[" ) );
    EXPECT_EQ( boost::none, parse_json( "[null" ) );
    EXPECT_EQ( boost::none, parse_json( "null]" ) );
    EXPECT_EQ( boost::none, parse_json( "]" ) );
    EXPECT_EQ( boost::none, parse_json( "[null null]" ) );
}