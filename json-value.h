#pragma  once

#include <iosfwd>
#include <vector>

#include <boost/variant.hpp>

namespace pyjamas
{
    struct null {};
    inline bool operator==( null, null ) { return true; }
    inline bool operator!=( null, null ) { return false; }
    std::ostream& operator<<( std::ostream& os, null );

    struct boolean {
        bool value;
    };
    inline bool operator==( boolean a, boolean b ) { return a.value == b.value; }
    inline bool operator!=( boolean a, boolean b ) { return a.value != b.value; }
    std::ostream& operator<<( std::ostream& os, const boolean& b );

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
        using value_type = boost::variant< null, boolean, array >;

        template< class T >
        JsonValue( value_type v ): value{ std::move( v )}
        {}

        JsonValue( null    n ): value{ n } {}
        JsonValue( bool    b ): value{ boolean{ b }} {}
        JsonValue( boolean b ): value{ b } {}
        JsonValue( array   a ): value{ std::move( a )} {}

        value_type value;
    };

    bool operator==( const JsonValue& a, const JsonValue& b );
    std::ostream& operator<<( std::ostream& os, const JsonValue& v );
}
