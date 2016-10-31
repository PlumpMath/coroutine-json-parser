#include "json-value.h"

#include <iomanip>
#include <iostream>

#include <boost/range/algorithm/equal.hpp>

namespace pyjamas
{
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

}
