#pragma  once

#include "json-value.h"
#include "token.h"

#include <iosfwd>
#include <memory>
#include <vector>

#include <boost/variant.hpp>
#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace pyjamas
{
    struct JsonValue;

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

    private:
        OutChannelWriter writer;
    };


    // -------------------------------------------------------------------------
    //
    auto parse_json( std::istream& input ) -> boost::optional<JsonValue>;

    auto parse_json( const char* string ) -> boost::optional<JsonValue>;

    // -------------------------------------------------------------------------
    // implementation details

    auto get_tokens( std::istream& input ) -> std::vector< Token >;

    auto get_tokens( const char* string ) -> std::vector< Token >;
}
