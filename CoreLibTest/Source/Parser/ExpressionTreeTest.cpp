/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <gtest/gtest.h>

namespace
{
    using Node    = A1::Node;
    using NodePtr = A1::Node::Pointer;

    void match( NodePtr const & actual, NodePtr const & expected )
    {
        ASSERT_EQ( actual->value(), expected->value() );

        auto const & actualChildren  { actual  ->children() };
        auto const & expectedChildren{ expected->children() };

        auto const actualChildrenCount  { std::size( actualChildren   ) };
        auto const expectedChildrenCount{ std::size( expectedChildren ) };

        ASSERT_EQ( actualChildrenCount, expectedChildrenCount )
            << "Actual node children count: "     << actualChildrenCount
            << ", expected node children count: " << expectedChildrenCount;

        for ( auto i{ 0U }; i < actualChildrenCount; ++i )
        {
            match( actualChildren[ i ], expectedChildren[ i ] );
        }
    }

    void matchParsing( std::string_view const expression, NodePtr expectedRoot )
    {
        auto tokenIt   { A1::tokenize( A1::PushBackStream{ expression } ) };
        auto actualRoot{ A1::parse( tokenIt ) };

        match( actualRoot, expectedRoot );
    }

    template< typename ... Ts >
    std::vector< NodePtr > makeChildren( Ts && ... nodes )
    {
        std::vector< NodePtr > children;
        children.reserve( sizeof...( Ts ) );

        (
            [ & ]
            {
                children.push_back( std::move( nodes ) );
            }(), ...
        );

        return children;
    }
} // namespace

TEST( ExpressionTreeTest, parsing )
{
    {
        constexpr auto expression{ "var = 5" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchParsing
            (
                expression,
                std::make_unique< Node >
                (
                    A1::OperatorType::Assign,
                    makeChildren
                    (
                        std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                        std::make_unique< Node >( A1::Number{ 5 }                 )
                    )
                )
            )
        ) << "Parsing failure: '" << expression << "'";
    }
    {
        constexpr auto expression{ "func(1.4, \"This is random string\", 5)" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchParsing
            (
                expression,
                std::make_unique< Node >
                (
                    A1::OperatorType::Call,
                    makeChildren
                    (
                        std::make_unique< Node >( A1::Identifier{ .name = "func" }      ),
                        std::make_unique< Node >( A1::Number{ 1.4 }                     ),
                        std::make_unique< Node >( A1::String{ "This is random string" } ),
                        std::make_unique< Node >( A1::Number{ 5 }                       )
                    )
                )
            )
        ) << "Parsing failure: '" << expression << "'";
    }
    {
        constexpr auto expression{ "arr[1]" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchParsing
            (
                expression,
                std::make_unique< Node >
                (
                    A1::OperatorType::Index,
                    makeChildren
                    (
                        std::make_unique< Node >( A1::Identifier{ .name = "arr" } ),
                        std::make_unique< Node >( A1::Number{ 1 }                 )
                    )
                )
            )
        ) << "Parsing failure: '" << expression << "'";
    }
    {
        constexpr auto expression
        {
            "if var == 5:\n"
            "    equal = 1\n"
            "else:\n"
            "    equal = 2"
        };
        EXPECT_NO_FATAL_FAILURE
        (
            matchParsing
            (
                expression,
                std::make_unique< Node >
                (
                    A1::OperatorType::StatementIf,
                    makeChildren
                    (
                        std::make_unique< Node >
                        (
                            A1::OperatorType::Equality,
                            makeChildren
                            (
                                std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                std::make_unique< Node >( A1::Number{ 5 }                 )
                            )
                        ),
                        std::make_unique< Node >
                        (
                            A1::OperatorType::Assign,
                            makeChildren
                            (
                                std::make_unique< Node >( A1::Identifier{ .name = "equal" } ),
                                std::make_unique< Node >( A1::Number{ 1 }                   )
                            )
                        ),
                        std::make_unique< Node >
                        (
                            A1::OperatorType::Assign,
                            makeChildren
                            (
                                std::make_unique< Node >( A1::Identifier{ .name = "equal" } ),
                                std::make_unique< Node >( A1::Number{ 2 }                   )
                            )
                        )
                    )
                )
            )
        ) << "Parsing failure: '" << expression << "'";
    }
}