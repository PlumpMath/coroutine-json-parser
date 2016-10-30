This is an experiment to create a JSON parser using coroutines to effect the
concurrent execution of lexer and parser.

It uses the asymmetric coroutine support provided by
[Boost.Coroutine](http://www.boost.org/doc/libs/1_58_0/libs/coroutine/doc/html/coroutine/coroutine/asymmetric.html).

Build currently requires Make and a g++ implementation capable of building in
C++11 mode; tested with 5.4.1.
Tests require Google Test; can't find the version right now. Probably 1.7.

Just a pet project to try out coroutines: it's a work in progress, will probably
end up gathering dust around 70% completion, may be questionable in places, and
probably incompatible with your health insurance.

It's unlikely to destroy your hard drive, but caveats right? So it might :-P
