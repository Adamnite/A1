/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <cstdlib>

namespace A1
{

namespace
{
    enum class CharType : std::uint8_t
    {
        Comment,
        Quote,
        Whitespace,
        Alphanumeric,
        Operator
    };

    [[ nodiscard ]] CharType getCharType( int const c ) noexcept
    {
             if ( c == '#'              ) { return CharType::Comment;      }
        else if ( c == '"' || c == '\'' ) { return CharType::Quote;        }
        else if ( std::isspace( c )     ) { return CharType::Whitespace;   }
        else if ( std::isalnum( c )     ) { return CharType::Alphanumeric; }
        else                              { return CharType::Operator;     }
    }

    void skipComment( PushBackStream & stream ) noexcept
    {
        auto c{ stream.pop() };
        while ( c && *c != '\n' ) { c = stream.pop(); }

        if ( c && *c != '\n' )
        {
            stream.push( *c );
        }
    }

    [[ nodiscard ]] Token getWord( PushBackStream & stream )
    {
        std::string result;

        for
        (
            auto c{ stream.pop() };
            c && ( getCharType( *c ) == CharType::Alphanumeric || *c == '.' || *c == '_' );
            c = stream.pop()
        )
        {
            result.push_back( static_cast< char >( *c ) );
        }

        if ( auto const keyword{ getKeyword( result ) }; keyword != ReservedToken::Unknown )
        {
            return Token{ keyword, stream.lineNumber(), stream.charIndex() };
        }
        else
        {
            if ( std::isdigit( result.front() ) )
            {
                char * endPtr{ nullptr };
                double number{ std::strtod( result.data(), &endPtr ) };

                if ( endPtr == result.data() )
                {
                    return {};
                    // TODO: Throw an unexpected error instead here
                }

                return Token{ number, stream.lineNumber(), stream.charIndex() };
            }

            return Token{ Identifier{ std::move( result ) }, stream.lineNumber(), stream.charIndex() };
        }
    }

    [[ nodiscard ]] Token getString( PushBackStream & stream )
    {
        std::string result;

        auto escaped{ false };

        while ( auto const c{ stream.pop().value_or( -1 ) } )
        {
            if ( c == '\\' )
            {
                escaped = true;
                continue;
            }

            if ( escaped )
            {
                switch( c )
                {
                    case 't': result.push_back( '\t' ); break;
                    case 'n': result.push_back( '\n' ); break;
                    case 'r': result.push_back( '\r' ); break;
                    case '0': result.push_back( '\0' ); break;
                    default : result.push_back( c    ); break;
                }
                escaped = false;
            }
            else if ( c == '"' )
            {
                // we have read the closing quote, thus we have read the word
                return Token{ std::move( result ), stream.lineNumber(), stream.charIndex() };
            }
            else
            {
                result.push_back(c);
            }
        }

        return {};
        // TODO: Throw a parsing error instead
    }

    [[ nodiscard ]] Token tokenizeImpl( PushBackStream & stream )
    {
        while ( auto const c{ stream.pop() } )
        {
            switch ( getCharType( *c ) )
            {
                case CharType::Whitespace:
                    continue;

                case CharType::Comment:
                    skipComment(stream);
                    continue;

                case CharType::Alphanumeric:
                    stream.push( *c );
                    return getWord( stream );

                case CharType::Quote:
                    return getString( stream );

                case CharType::Operator:
                    stream.push( *c );

                    auto const op{ getOperator( stream ) };
                    if ( op == ReservedToken::Unknown )
                    {
                        // TODO: Throw parsing error
                        return {};
                    }
                    return { op, stream.lineNumber(), stream.charIndex() };
            }
        }

        return { Eof{}, stream.lineNumber(), stream.charIndex() };
    }
} // namespace

TokenIterator & TokenIterator::operator++()
{
    current_ = tokenizeImpl( stream_ );
    return *this;
}

TokenIterator TokenIterator::operator++(int)
{
    auto tmp{ *this };
    ++*this;
    return tmp;
}

TokenIterator tokenize( PushBackStream stream )
{
    return { std::move( stream ) };
}

} // namespace A1