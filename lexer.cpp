#include "lexer.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <string>

namespace pyjamas
{

Lexer::Lexer( std::istream& in, OutChannelWriter writer ):
    in{ in },
    eof{ false },
    writer{ std::move( writer )}
{
    consume_char();
}

void Lexer::run()
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


void Lexer::get_string()
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

void Lexer::get_keyword()
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

void Lexer::consume_char()
{
    in.get( c );
    if (!in)
        eof = true;
}

}
