#include "json-parser.h"
#include "lexer.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/adaptor/indirected.hpp>

namespace pyjamas
{

Parser::Parser( OutChannelWriter writer ):
    writer{ std::move( writer )}
{}

auto Parser::channel_writer() -> InChannelWriter
{
    return InChannelWriter{
        [this](InChannelReader& reader) {
            run( reader );
        }};
}

void Parser::run( InChannelReader& reader )
{
    while (reader)
    {
        auto token = reader.get();
        switch (token.type)
        {
            case TokenType::Null:
                writer( null{} );
                break;

            case TokenType::ArrayBegin:
                reader();
                if (!reader)
                {
                    std::cerr << "Expected array-element or array-end but no more tokens" << std::endl;
                    return;
                }

                token = reader.get();
                if (token.type != TokenType::ArrayEnd)
                {
                    std::cerr << "Expected array-end but got something else" << std::endl;
                    return;
                }
                writer( JsonValue{ array{} });
                break;

            default:
                writer( JsonValue{ null{} });
        }

        reader();
    }
}

// TODO; make this take a coroutine to give tokens to
// or a channel of some sort? signal slot? fiber?
auto get_tokens( std::istream& input ) -> std::vector< Token >
{
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

auto parse_json( std::istream& input ) -> JsonValue
{
    JsonValue value{ null{} };

    Parser parser( Parser::OutChannelWriter{ [&](Parser::OutChannelReader& reader) {
        while (reader)
        {
            value = reader.get();
            reader();
        }
    }} );

    Lexer lex{ input, parser.channel_writer() };
    lex.run();

    return value;
}

namespace {
    struct JsonValueEqualityComparator
    {
        template<class T>
        bool operator()( const T& a, const T& b ) const
        {
            return a == b;
        }
    };
}

bool operator==( const JsonValue& a, const JsonValue& b )
{
    return a.value == b.value;
}

bool operator==( const array& a, const array& b )
{
    return boost::equal(
        a.children | boost::adaptors::indirected,
        b.children | boost::adaptors::indirected );
}

bool operator!=( const array& a, const array& b )
{
    return !(a == b);
}

std::ostream& operator<<( std::ostream& os, const array& arr )
{
    os << "[";

    bool first = true;
    for (const auto& v : arr.children)
    {
        if (first)
            first = false;
        else
            os << ", ";
        os << *v;
    }
    return os << "]";
}

namespace {
    struct JsonValuePrinter
    {
        void operator()( const array& v ) const
        {
            *out << v;
        }

        void operator()( null ) const
        {
            *out << "null";
        }

        std::ostream* out;
    };
}

std::ostream& operator<<( std::ostream& os, const JsonValue& v )
{
    boost::apply_visitor( JsonValuePrinter{ &os }, v.value );
    return os;
}

auto get_tokens( const char* string ) -> std::vector< Token >
{
    std::istringstream in{ string };
    return get_tokens( in );
}

auto parse_json( const char* string ) -> JsonValue
{
    std::istringstream in{ string };
    return parse_json( in );
}
}
