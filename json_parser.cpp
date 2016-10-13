#include "catch.hpp"

#include <iostream>
#include <sstream>
#include <string>


enum class Token
{
    End
};

struct JSONParser
{
    JSONParser( std::istream&& in ):
            in( in )
    {}

    auto get_token() -> Token
    {

        return Token::End;
    }

private:
    std::istream&  in;
};


auto get_token( const char* string ) -> Token
{
    return JSONParser{
        std::istringstream{
            string
        }
    }.get_token();
}

TEST_CASE( "Consumes null", "[json_parser]" )
{
    REQUIRE( get_token( "" ) == Token::End );
}
