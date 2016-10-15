#pragma  once
#include <iosfwd>
#include <vector>

namespace pyjamas
{
    enum class Token
    {
        None,
        End,
        Null,
        True,
        False
    };

    std::ostream& operator<<( std::ostream& os, Token tok );

    auto get_tokens( const char* string ) -> std::vector< Token >;

    struct JSONParser
    {
        JSONParser( std::istream& in );

        auto get_token() -> Token;

    private:
        auto get_keyword() -> Token;

        void consume_char();

        std::istream&  in;
        char           c;
        bool           error;
        Token          token; //< Result
    };
}
