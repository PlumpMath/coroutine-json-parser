#pragma  once

#include "token.h"

#include <iosfwd>
#include <memory>
#include <vector>

#include <boost/variant.hpp>
#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace pyjamas
{
    struct null {};
    bool operator==( null, null ) { return true; }
    bool operator!=( null, null ) { return false; }

    struct JsonValue;

    struct array {
        using child_container = std::vector< JsonValue >;
        child_container children;
    };

    bool operator==( const array&, const array& );
    bool operator!=( const array&, const array& );
    std::ostream& operator<<( std::ostream& os, const array& v );

    struct JsonValue
    {
        using value_type = boost::variant< null, array >;

        template< class T >
        JsonValue( value_type v ): value{ std::move( v )}
        {}

        JsonValue( null  n ): value{ n } {}
        JsonValue( array a ): value{ std::move( a )} {}

        value_type value;
    };

    bool operator==( const JsonValue& a, const JsonValue& b );
    std::ostream& operator<<( std::ostream& os, const JsonValue& v );

    // -------------------------------------------------------------------------
    // The parser
    struct Parser
    {
        using InChannel        = boost::coroutines::asymmetric_coroutine< Token >;
        using InChannelWriter  = InChannel::push_type;
        using InChannelReader  = InChannel::pull_type;

        using OutChannel       = boost::coroutines::asymmetric_coroutine< JsonValue >;
        using OutChannelWriter = OutChannel::push_type;
        using OutChannelReader = OutChannel::pull_type;

        Parser( OutChannelWriter writer );

        auto channel_writer() -> InChannelWriter;

        void run( InChannelReader& reader );

    private:
        OutChannelWriter writer;
    };


    // -------------------------------------------------------------------------
    //
    auto parse_json( std::istream& input ) -> JsonValue;

    auto parse_json( const char* string ) -> JsonValue;

    // -------------------------------------------------------------------------
    // implementation details

    auto get_tokens( std::istream& input ) -> std::vector< Token >;

    auto get_tokens( const char* string ) -> std::vector< Token >;
}
