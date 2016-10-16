#include "json_parser.h"
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace pyjamas
{

std::ostream& operator<<( std::ostream& os, Token tok )
{
    switch (tok)
    {
        case Token::Invalid: return os << "Invalid";
        case Token::Null:    return os << "Null";
        case Token::True:    return os << "True";
        case Token::False:   return os << "False";
        default:             return os << "<- ERROR: unknown Token ->";
    }
}


namespace
{
    struct Lexer
    {
        using OutChannel       = boost::coroutines::asymmetric_coroutine< Token >;
        using OutChannelWriter = OutChannel::push_type;
        using OutChannelReader = OutChannel::pull_type;

        Lexer( std::istream& in, OutChannelWriter writer ):
            in{ in },
            eof{ false },
            writer{ std::move( writer )}
        {
            consume_char();
        }

        void run()
        {
            while (!eof)
            {
                if (std::isspace( c ))
                    consume_char();
                else if (std::isalpha( c ))
                    get_keyword();
                else
                    std::cout << "Invalid character " << c << " (" << int(c) << ")\n";
            }
        }

    private:
        void get_keyword()
        {
            assert( !eof              && "get_keyword expected !EOF");
            assert( std::isalpha( c ) && "get_keyword expected an alpha character");

            std::string token;
            do
            {
                token += c;
                consume_char();
            } while (!eof && std::isalpha( c ));

            if (token == "null")
                writer( Token::Null );
            else if (token == "true")
                writer( Token::True );
            else if (token == "false")
                writer( Token::False );
            else
            {
                std::cerr << "Invalid token '" << token << "'\n";
                writer( Token::Invalid );
            }
        }

        void consume_char()
        {
            in.get( c );
            if (!in)
                eof = true;
        }

        std::istream&     in;
        char              c;
        bool              eof;
        OutChannelWriter  writer;
    };
} // unnamed NS


// TODO; make this take a coroutine to give tokens to
// or a channel of some sort? signal slot? fiber?
auto get_tokens( const char* string ) -> std::vector< Token >
{
    std::istringstream input{ string };
    auto tokens = std::vector< Token >{};

    Lexer lex( input, Lexer::OutChannelWriter{ [&](Lexer::OutChannelReader& reader) {
        while (reader)
        {
            auto token = reader.get();
            tokens.push_back( token );
            reader();
        }
    }} );

    lex.run();

    return tokens;
}

}
