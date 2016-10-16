#pragma  once
#include <iosfwd>
#include <string>
#include <vector>

#include <boost/optional.hpp> // TODO: to std::optional?

namespace pyjamas
{
    enum class TokenType
    {
        Invalid,
        Null,
        True,
        False,
        String,
        ArrayBegin,
        ArrayEnd,
        ItemSeparator
    };

    struct Token
    {
        Token( TokenType t ):
            type{ t }
        {}

        Token( TokenType t, std::string text ):
            type{ t },
            text{ std::move( text )}
        {}

        TokenType type;
        boost::optional< std::string > text;
    };


    bool operator==( const Token& a, const Token& b );

    std::ostream& operator<<( std::ostream& os, const Token& tok );

    auto get_tokens( const char* string ) -> std::vector< Token >;
}
