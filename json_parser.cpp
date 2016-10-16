#include "json_parser.h"
#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <boost/optional/optional_io.hpp>

namespace pyjamas
{

std::ostream& operator<<( std::ostream& os, const Token& tok )
{
    switch (tok.type)
    {
        case TokenType::Invalid:        return os << "Invalid \"" << tok.text << '"';
        case TokenType::Null:           return os << "Null";
        case TokenType::True:           return os << "True";
        case TokenType::False:          return os << "False";
        case TokenType::String:         os << "String " << &tok << " \"" << std::flush; return os << *tok.text << '"';
        case TokenType::ArrayBegin:     return os << "ArrayBegin";
        case TokenType::ArrayEnd:       return os << "ArrayEnd";
        case TokenType::ItemSeparator:  return os << "ItemSeparator";
        default:                        return os << "<- ERROR: unknown Token ->";
    }
}


bool operator==( const Token& a, const Token& b )
{
    return
        a.type == b.type &&
        a.text == b.text;
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
                {
                    switch (c)
                    {
                        case '[': writer( TokenType::ArrayBegin );    break;
                        case ']': writer( TokenType::ArrayEnd );      break;
                        case ',': writer( TokenType::ItemSeparator ); break;
                        case '"': get_string(); break;
                        default:
                            std::cout << "Invalid character " << c << " (" << int(c) << ")\n";
                    }
                    consume_char();
                }
            }
        }

    private:
        void get_string()
        {
            assert( !eof     && "get_string expected !EOF");
            assert( c == '"' && "get_string expected '\"'");

            consume_char();

            std::string string;
            while (!eof && c != '"')
            {
                string += c;
                consume_char();
            }

            if (eof)
                writer( Token{ TokenType::Invalid, "Unterminated string" });
            else
            {
                writer( Token{ TokenType::String, std::move( string )});
                assert( c == '"' );
                consume_char();
            }
        }

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
                writer( TokenType::Null );
            else if (token == "true")
                writer( TokenType::True );
            else if (token == "false")
                writer( TokenType::False );
            else
            {
                std::cerr << "Invalid token '" << token << "'\n";
                writer( TokenType::Invalid );
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
