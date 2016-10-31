#include "json-parser.h"
#include "lexer.h"

#include <cassert>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/coroutine/asymmetric_coroutine.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/equal.hpp>

namespace pyjamas
{
namespace
{
    /// Returns a JSON value by parsing tokens yielded from the given tokenChannel.
    ///
    /// On success the value will be returned, and the tokenChannel will either have finished
    /// producing tokens or the next token will be available through it.
    ///
    /// On error boost::none will be returned; the tokenChannel state is unspecified.
    auto get_value( Parser::InChannelReader& tokenChannel ) -> boost::optional< JsonValue >
    {
        if (!tokenChannel)
        {
            std::cerr << "Token source has dried up!" << std::endl;
            return boost::none;
        }

        // TODO: use a sub-function that returns an optional Token to wrap up the
        //       repeated [check tokenChannel, print message, get token] sequence

        auto token = tokenChannel.get();
        switch (token.type)
        {
            case TokenType::Null:
                return JsonValue{ null{} };

            case TokenType::ArrayBegin:
            {
                tokenChannel();

                array arrayValue;
                while (tokenChannel && tokenChannel.get().type != TokenType::ArrayEnd)
                {
                    auto element = get_value( tokenChannel );
                    if (!element)
                        return element;
                    arrayValue.children.emplace_back( std::move( *element ));

                    tokenChannel();
                    if (!tokenChannel)
                    {
                        std::cerr << "Expected item-separator or array-end but no more tokens" << std::endl;
                        return boost::none;
                    }

                    if (tokenChannel.get().type == TokenType::ItemSeparator)
                    {
                        tokenChannel();
                        if (tokenChannel.get().type == TokenType::ArrayEnd)
                        {
                            std::cerr << "Expected array-element after item-separator but got array-end" << std::endl;
                            return boost::none;
                        }
                    }
                    else if (tokenChannel.get().type != TokenType::ArrayEnd)
                    {
                        std::cerr << "Expected array-end after array-element" << std::endl;
                        return boost::none;
                    }
                }

                if (!tokenChannel)
                {
                    std::cerr << "Expected array-element or array-end but no more tokens" << std::endl;
                    return boost::none;
                }

                assert( tokenChannel.get().type == TokenType::ArrayEnd );
                return JsonValue{ std::move( arrayValue )};
            }

            case TokenType::True:
                return JsonValue{ true };

            case TokenType::False:
                return JsonValue{ false };

            case TokenType::ArrayEnd:
            case TokenType::ItemSeparator:
                std::cerr << "Unexpected token " << token << std::endl;
                // fall-through to next case

            case TokenType::Invalid:
                return boost::none;

            default:
                std::cerr << "Invalid/unknown token " << token << std::endl;
                return boost::none;
        }
    }
} // unnamed NS

Parser::Parser( OutChannelWriter writer ):
    writer{ std::move( writer )}
{}

auto Parser::channel_writer() -> InChannelWriter
{
    return InChannelWriter{
        [this](InChannelReader& reader) {
            if (!writer)
            {
                std::cerr << "Parser cannot operate: output channel closed" << std::endl;
                return;
            }

            auto value = get_value( reader );
            if (value)
                writer( std::move( *value ));
        }};
}


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

auto parse_json( std::istream& input ) -> boost::optional< JsonValue >
{
    boost::optional<JsonValue> value;

    Parser parser( Parser::OutChannelWriter{ [&](Parser::OutChannelReader& reader) {
        if (reader)
            value = reader.get();
    }} );

    Lexer lex{ input, parser.channel_writer() };
    lex.run();

    if (!lex.at_end())
    {
        std::cerr << "Finished parsing but still have trailing characters\n";
        return boost::none;
    }
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
    return boost::equal( a.children, b.children );
}

bool operator!=( const array& a, const array& b )
{
    return !(a == b);
}

std::ostream& operator<<( std::ostream& os, null )
{
    return os << "null";
}

std::ostream& operator<<( std::ostream& os, const boolean& b )
{
    return os << std::boolalpha << b.value;
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
        os << v;
    }
    return os << "]";
}

namespace {
    struct JsonValuePrinter
    {
        template< class T >
        void operator()( const T& v ) const
        {
            *out << v;
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

auto parse_json( const char* string ) -> boost::optional<JsonValue>
{
    std::istringstream in{ string };
    return parse_json( in );
}
}
