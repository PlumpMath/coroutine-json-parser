#pragma  once

#include "token.h"

#include <iosfwd>
#include <boost/coroutine/asymmetric_coroutine.hpp>

namespace pyjamas
{
    struct Lexer
    {
        using OutChannel       = boost::coroutines::asymmetric_coroutine< Token >;
        using OutChannelWriter = OutChannel::push_type;
        using OutChannelReader = OutChannel::pull_type;

        Lexer( std::istream& in, OutChannelWriter writer );

        void run();

    private:
        void get_string();

        void get_keyword();

        void consume_char();

        std::istream&     in;
        char              c;
        bool              eof;
        OutChannelWriter  writer;
    };
}
