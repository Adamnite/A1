/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <charconv>
#include <cstdlib>

namespace A1
{

namespace
{
    enum class CharType : std::uint8_t
    {
        Alphanumeric,
        Comment,
        Newline,
        Operator,
        Quote,
        Tab,
        Whitespace
    };

    [[ nodiscard ]] CharType getCharType( int const c ) noexcept
    {
             if ( std::isalnum( c )     ) { return CharType::Alphanumeric; }
             if ( c == '#'              ) { return CharType::Comment;      }
        else if ( c == '\n'             ) { return CharType::Newline;      }
        else if ( c == '"' || c == '\'' ) { return CharType::Quote;        }
        else if ( c == '\t'             ) { return CharType::Tab;          }
        else if ( std::isspace( c )     ) { return CharType::Whitespace;   }
        else                              { return CharType::Operator;     }
    }

    void skipComment( PushBackStream & stream ) noexcept
    {
        auto c{ stream.pop() };

        while ( c && *c != '\n' ) { c = stream.pop(); }

        if ( c ) { stream.push( *c ); }
    }

    [[ nodiscard ]] Token getWord( PushBackStream & stream )
    {
        std::string result;

        std::optional< int > lastChar;

        auto isNumber{ true };

        for
        (
            lastChar = stream.pop();
            lastChar && ( getCharType( *lastChar ) == CharType::Alphanumeric || *lastChar == '.' || *lastChar == '_' );
            lastChar = stream.pop()
        )
        {
            if ( !std::isdigit( *lastChar ) && *lastChar != '.' )
            {
                isNumber = false;
            }

            if ( !isNumber && *lastChar == '.' )
            {
                break;
            }
            result.push_back( static_cast< char >( *lastChar ) );
        }

        // return last character back to the stream
        if ( lastChar ) { stream.push( *lastChar ); }

        if ( auto const keyword{ getKeyword( result ) }; keyword != ReservedToken::Unknown )
        {
            return Token{ keyword, stream.errorInfo() };
        }
        else
        {
            if ( std::isdigit( result.front() ) )
            {
                Number number{ 0U };
                if
                (
                    auto const [ ptr, ec ]{ std::from_chars( result.data(), result.data() + result.size(), number ) };
                    ec != std::errc{}
                )
                {
                    return {};
                    // TODO: Throw an unexpected error instead here
                }

                return Token{ number, stream.errorInfo() };
            }

            return Token{ Identifier{ std::move( result ) }, stream.errorInfo() };
        }
    }

    [[ nodiscard ]] Token getString( PushBackStream & stream )
    {
        String result;

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
                return Token{ std::move( result ), stream.errorInfo() };
            }
            else
            {
                result.push_back(c);
            }
        }

        // TODO: Throw a parsing error
        return {};
    }

    [[ nodiscard ]] Token tokenizeImpl( PushBackStream & stream )
    {
        static constexpr auto whitespacesIndentationCount{ 4U };
        auto consecutiveWhitespacesCount{ 0U };

        while ( auto const c{ stream.pop() } )
        {
            switch ( getCharType( *c ) )
            {
                case CharType::Alphanumeric:
                {
                    consecutiveWhitespacesCount = 0U;
                    stream.push( *c );
                    return getWord( stream );
                }
                case CharType::Comment:
                {
                    consecutiveWhitespacesCount = 0U;
                    skipComment( stream );
                    continue;
                }
                case CharType::Newline:
                {
                    consecutiveWhitespacesCount = 0U;
                    return { Newline{}, stream.errorInfo() };
                }
                case CharType::Tab:
                {
                    consecutiveWhitespacesCount = 0U;
                    return { Indentation{}, stream.errorInfo() };
                }
                case CharType::Quote:
                {
                    consecutiveWhitespacesCount = 0U;
                    return getString( stream );
                }
                case CharType::Whitespace:
                {
                    consecutiveWhitespacesCount++;
                    if ( consecutiveWhitespacesCount == whitespacesIndentationCount )
                    {
                        return { Indentation{}, stream.errorInfo() };
                    }
                    continue;
                }
                case CharType::Operator:
                {
                    consecutiveWhitespacesCount = 0U;
                    stream.push( *c );

                    auto const op{ getOperator( stream ) };
                    if ( op == ReservedToken::Unknown )
                    {
                        // TODO: Throw parsing error
                        return {};
                    }
                    return { op, stream.errorInfo() };
                }
            }
        }

        return { Eof{}, stream.errorInfo() };
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