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
    using NodePtr = A1::Node const *;

    void match( NodePtr const actual, NodePtr const expected )
    {
        EXPECT_EQ( actual->value(), expected->value() );

        auto const & actualChildren  { actual  ->children() };
        auto const & expectedChildren{ expected->children() };

        auto const actualChildrenCount  { std::size( actualChildren   ) };
        auto const expectedChildrenCount{ std::size( expectedChildren ) };

        ASSERT_EQ( actualChildrenCount, expectedChildrenCount )
            << "Actual node children count: "     << actualChildrenCount
            << ", expected node children count: " << expectedChildrenCount;

        for ( auto i{ 0U }; i < actualChildrenCount; ++i )
        {
            match( actualChildren[ i ].get(), expectedChildren[ i ].get() );
        }
    }

    template< typename ... Ts >
    std::vector< std::unique_ptr< Node > > makeChildren( Ts && ... nodes )
    {
        std::vector< std::unique_ptr< Node > > children;
        children.reserve( sizeof...( Ts ) );

        (
            [ & ]
            {
                children.push_back( std::move( nodes ) );
            }(), ...
        );

        return children;
    }

    struct TestParameter
    {
        std::string_view title;
        std::string_view expression;

        /**
         * NOTE: Needs to be std::shared_ptr instead of std::unique_ptr
         *       because GTest performs copy internally.
         */
        std::shared_ptr< Node > expectedRoot;

        friend std::ostream & operator<<( std::ostream & os, TestParameter const & param )
        {
            return os << param.expression;
        }
    };

    struct ExpressionTreeTestFixture : ::testing::TestWithParam< TestParameter >
    {
        struct PrintTitle
        {
            template< typename ParamType >
            std::string operator()( testing::TestParamInfo< ParamType > const & info ) const
            {
                auto parameter{ static_cast< TestParameter >( info.param ) };
                return std::string{ parameter.title };
            }
        };
    };
} // namespace

TEST_P( ExpressionTreeTestFixture, parsing )
{
    auto const [ _, expression, expectedRoot ]{ GetParam() };

    auto tokenIt   { A1::tokenize( A1::PushBackStream{ expression } ) };
    auto actualRoot{ A1::parse( tokenIt ) };

    match( actualRoot.get(), expectedRoot.get() );
}

INSTANTIATE_TEST_SUITE_P
(
    ExpressionTreeTest,
    ExpressionTreeTestFixture,
    ::testing::Values
    (
        TestParameter
        {
            .title        = "VariableAssignment",
            .expression   = "var = 5",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::Assign,
                makeChildren
                (
                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                    std::make_unique< Node >( A1::Number{ 5 }                 )
                )
            )
        },
        TestParameter
        {
            .title        = "FunctionCall",
            .expression   = "func(1.4, \"This is random string\", 5)",
            .expectedRoot = std::make_shared< Node >
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
        },
        TestParameter
        {
            .title        = "ArrayIndex",
            .expression   = "arr[1]",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::Index,
                makeChildren
                (
                    std::make_unique< Node >( A1::Identifier{ .name = "arr" } ),
                    std::make_unique< Node >( A1::Number{ 1 }                 )
                )
            )
        },
        TestParameter
        {
            .title        = "ReturnValue",
            .expression   = "return 1",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::StatementReturn,
                makeChildren
                (
                    std::make_unique< Node >( A1::Number{ 1 } )
                )
            )
        },
        TestParameter
        {
            .title        = "ReturnCondition",
            .expression   = "return a < b",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::StatementReturn,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        A1::OperatorType::LessThan,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "a" } ),
                            std::make_unique< Node >( A1::Identifier{ .name = "b" } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .title        = "Pass",
            .expression   = "pass",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::StatementPass
            )
        },
        TestParameter
        {
            .title      = "IfElseBranch",
            .expression =
                "if var == 5:\n"
                "    new_var = 1\n"
                "else:\n"
                "    new_var = 2",
            .expectedRoot = std::make_shared< Node >
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
                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                            std::make_unique< Node >( A1::Number{ 1 }                     )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        A1::OperatorType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                            std::make_unique< Node >( A1::Number{ 2 }                     )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .title      = "IfElifElseBranch",
            .expression =
                "if var == 5:\n"
                "    new_var = 1\n"
                "elif var == 6:\n"
                "    new_var = 2\n"
                "elif var == 7:\n"
                "    new_var = 3\n"
                "else:\n"
                "    new_var = 4",
            .expectedRoot = std::make_shared< Node >
            (
                // if branch
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
                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                            std::make_unique< Node >( A1::Number{ 1 }                     )
                        )
                    ),
                    // first elif branch
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
                                    std::make_unique< Node >( A1::Number{ 6 }                 )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                A1::OperatorType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                    std::make_unique< Node >( A1::Number{ 2 }                     )
                                )
                            ),
                            // second elif branch
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
                                            std::make_unique< Node >( A1::Number{ 7 }                 )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        A1::OperatorType::Assign,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                            std::make_unique< Node >( A1::Number{ 3 }                     )
                                        )
                                    ),
                                    // else branch
                                    std::make_unique< Node >
                                    (
                                        A1::OperatorType::Assign,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                            std::make_unique< Node >( A1::Number{ 4 }                     )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .title      = "WhileLoop",
            .expression =
                "while i < 5:\n"
                "    i = i + 1",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::StatementWhile,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        A1::OperatorType::LessThan,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                            std::make_unique< Node >( A1::Number{ 5 }               )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        A1::OperatorType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                            std::make_unique< Node >
                            (
                                A1::OperatorType::Addition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                    std::make_unique< Node >( A1::Number{ 1 }               )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .title      = "SmartContract",
            .expression =
                "contract Example:\n"
                "    pass",
            .expectedRoot = std::make_shared< Node >
            (
                A1::OperatorType::ClassDefinition,
                makeChildren
                (
                    std::make_unique< Node >( A1::Identifier{ .name = "Example" } ),
                    std::make_unique< Node >
                    (
                        A1::OperatorType::StatementPass
                    )
                )
            )
        }
    ),
    ExpressionTreeTestFixture::PrintTitle()
);