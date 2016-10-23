#include "token.h"

#include <iostream>
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

}