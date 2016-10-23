#pragma  once

#include "token.h"

#include <string>
#include <vector>

namespace pyjamas
{
    auto get_tokens( const char* string ) -> std::vector< Token >;
}
