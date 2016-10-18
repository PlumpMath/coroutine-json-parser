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
        case TokenType::Invalid:            return os << "Invalid \"" << tok.text << '"';
        case TokenType::Null:               return os << "Null";
        case TokenType::True:               return os << "True";
        case TokenType::False:              return os << "False";
        case TokenType::String:             return os << "String \"" << *tok.text << '"';
        case TokenType::ArrayBegin:         return os << "ArrayBegin";
        case TokenType::ArrayEnd:           return os << "ArrayEnd";
        case TokenType::ObjectBegin:        return os << "ObjectBegin";
        case TokenType::ObjectEnd:          return os << "ObjectEnd";
        case TokenType::ItemSeparator:      return os << "ItemSeparator";
        case TokenType::KeyValueSeparator:  return os << "KeyValueSeparator";
        default:                            return os << "<- ERROR: unknown token type (" << static_cast<int>(tok.type) << ") ->";
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
                        case '[': writer( Token::array_begin         );  consume_char();  break;
                        case ']': writer( Token::array_end           );  consume_char();  break;
                        case ',': writer( Token::item_separator      );  consume_char();  break;
                        case '{': writer( Token::object_begin        );  consume_char();  break;
                        case '}': writer( Token::object_end          );  consume_char();  break;
                        case ':': writer( Token::key_value_separator );  consume_char();  break;
                        case '"': get_string(); break;
                        default:
                            std::cout << "Invalid character " << c << " (" << int(c) << ")\n";
                            consume_char();
                    }
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
            bool invalidString = false;
            while (!eof && c != '"')
            {
                if (c == '\\')
                {
                    consume_char();
                    switch (c)
                    {
                        case '"':
                            /* c already == escape sequence production */
                            break;

                        case '\\':
                            /* c already == escape sequence production */
                            break;

                        case '/':
                            /* c already == escape sequence production */
                            break;

                        case 'b':
                            c = '\b';
                            break;

                        case 'f':
                            c = '\f';
                            break;

                        case 'n':
                            c = '\n';
                            break;

                        case 'r':
                            c = '\r';
                            break;

                        case 't':
                            c = '\t';
                            break;

                        default:
                            std::cout << "Unknown escape sequence \\" << c << std::endl;
                            if (!invalidString)
                            {
                                invalidString = true;
                                writer( Token{ Token::invalid, std::string{"Unknown escape sequence \\"} + c });
                            }
                    }
                }

                if (!invalidString)
                    string += c;
                consume_char();
            }

            if (eof)
                writer( Token{ Token::invalid, "Unterminated string" });
            else
            {
                if (!invalidString)
                    writer( Token{ std::move( string )});
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
                writer( Token::null );
            else if (token == "true")
                writer( true );
            else if (token == "false")
                writer( false );
            else
            {
                std::cerr << "Invalid token '" << token << "'\n";
                writer( Token::invalid );
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
