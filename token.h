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
        ObjectBegin,
        ObjectEnd,
        ItemSeparator,
        KeyValueSeparator
    };

    struct Token
    {
        /// tag types
        struct invalid_t {};
        struct null_t {};
        struct array_begin_t {};
        struct array_end_t {};
        struct object_begin_t {};
        struct object_end_t {};
        struct item_separator_t {};
        struct key_value_separator_t {};
        static constexpr null_t                null = {};
        static constexpr invalid_t             invalid = {};
        static constexpr array_begin_t         array_begin = {};
        static constexpr array_end_t           array_end = {};
        static constexpr object_begin_t        object_begin = {};
        static constexpr object_end_t          object_end = {};
        static constexpr item_separator_t      item_separator = {};
        static constexpr key_value_separator_t key_value_separator = {};


        Token( invalid_t = {}, std::string message = {} ):
            type{ TokenType::Invalid },
            text{ std::move( message )}
        {}

        Token( null_t ):                type{ TokenType::Null              } {}
        Token( array_begin_t ):         type{ TokenType::ArrayBegin        } {}
        Token( array_end_t ):           type{ TokenType::ArrayEnd          } {}
        Token( object_begin_t ):        type{ TokenType::ObjectBegin       } {}
        Token( object_end_t ):          type{ TokenType::ObjectEnd         } {}
        Token( item_separator_t ):      type{ TokenType::ItemSeparator     } {}
        Token( key_value_separator_t ): type{ TokenType::KeyValueSeparator } {}

        Token( bool b ):
            type{
                b ? TokenType::True : TokenType::False }
        {}

        Token( std::string text ):
            type{ TokenType::String },
            text{ std::move( text )}
        {}

        Token( const char* text ):
            Token( std::string{ text })
        {}

        TokenType type;
        boost::optional< std::string > text;
    };


    bool operator==( const Token& a, const Token& b );

    std::ostream& operator<<( std::ostream& os, const Token& tok );
}
