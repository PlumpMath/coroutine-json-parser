#include "json-parser.h"

#include <gtest/gtest.h>

using namespace pyjamas;

TEST(ut_json_parser, null )
{
    EXPECT_EQ( parse_json( "null" ), JsonValue( null{} ) );
}

TEST(ut_json_parser, array )
{
    EXPECT_EQ( JsonValue( array{} ), parse_json( "[]"  ) );
    EXPECT_EQ( JsonValue( array{} ), parse_json( "[]"  ) );
    EXPECT_EQ( JsonValue( array{} ), parse_json( "[]"  ) );
    EXPECT_EQ( JsonValue( array{} ), parse_json( "[ ]" ) );
}