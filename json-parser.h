#pragma  once

#include "token.h"

#include <iosfwd>
#include <vector>

namespace pyjamas
{
    auto get_tokens( std::istream& input ) -> std::vector< Token >;

    auto get_tokens( const char* string ) -> std::vector< Token >;
}
