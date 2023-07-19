/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/AST/AST.hpp>
#include <CoreLib/AST/ASTPrinter.hpp>
#include <CoreLib/Errors/ParsingError.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <gtest/gtest.h>

namespace
{
    using Node     = A1::AST::Node;
    using NodeType = A1::AST::NodeType;
    using NodePtr  = A1::AST::Node const *;

    void match( NodePtr const actual, NodePtr const expected )
    {
        if ( actual == nullptr || expected == nullptr )
        {
            ASSERT_EQ( actual, expected );
            return;
        }

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

    struct ASTTestFixture : ::testing::TestWithParam< TestParameter > {};
} // namespace

TEST_P( ASTTestFixture, parsing )
{
    auto const [ expression, expectedRoot ]{ GetParam() };

    auto token     { A1::tokenize( A1::Stream{ expression } ) };
    auto actualRoot{ A1::AST::parse( token ) };

    match( actualRoot.get(), expectedRoot.get() );

    A1::AST::print( actualRoot );
}

INSTANTIATE_TEST_SUITE_P
(
    ASTTest,
    ASTTestFixture,
    ::testing::Values
    (
        TestParameter
        {
            .expression   = "",
            .expectedRoot = std::make_shared< Node >( NodeType::ModuleDefinition )
        },
        TestParameter
        {
            .expression =
                "# First comment line\n"
                "# Second comment line\n",
            .expectedRoot = std::make_shared< Node >( NodeType::ModuleDefinition )
        },
        TestParameter
        {
            .expression   = "var = 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var = 5 + 5 + 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Addition,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var = (5 + 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Parentheses,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                            std::make_unique< Node >( A1::Number{ .value = 5 } )
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
            .expression   = "var = (5 + 5) * 2",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Multiplication,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Parentheses,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::Addition,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                                )
                                            )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 2 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var = 2 * (5 + 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Multiplication,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Number{ .value = 2 } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Parentheses,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::Addition,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                                )
                                            )
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
            .expression   = "var = (5 + 5) * (5 + 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Multiplication,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Parentheses,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::Addition,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                                )
                                            )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Parentheses,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::Addition,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                                )
                                            )
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
            .expression   = "var = get()",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "get" } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "func(1.4, \"This is random string\", 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Call,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >( A1::Number{ .value = 1 } ),
                            std::make_unique< Node >( A1::StringLiteral{ .value = "This is random string" } ),
                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var.member",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::MemberCall,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var"    } ),
                            std::make_unique< Node >( A1::Identifier{ .name = "member" } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var.member = 1 + 2",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::MemberCall,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var"    } ),
                                    std::make_unique< Node >( A1::Identifier{ .name = "member" } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Addition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Number{ .value = 1 } ),
                                    std::make_unique< Node >( A1::Number{ .value = 2 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "print(func(1.4, \"This is random string\", 5))",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Call,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 1 } ),
                                    std::make_unique< Node >( A1::StringLiteral{ .value = "This is random string" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var[1]",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Index,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >( A1::Number{ .value = 1 }        )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var[addr]",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Index,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var"  } ),
                            std::make_unique< Node >( A1::Identifier{ .name = "addr" } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "var[func()]",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Index,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "func" } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "func1(func2() + 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Call,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func1" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Addition,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Call,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "func2" } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "func1(func2(5) + 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::Call,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func1" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Addition,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Call,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "func2" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "return 1",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementReturn,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Number{ .value = 1 } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "import core",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementImport,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "core" } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "assert a < b",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementAssert,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::LessThan,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "a" } ),
                                    std::make_unique< Node >( A1::Identifier{ .name = "b" } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "return a < b",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementReturn,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::LessThan,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "a" } ),
                                    std::make_unique< Node >( A1::Identifier{ .name = "b" } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "pass",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >( NodeType::StatementPass )
                )
            )
        },
        TestParameter
        {
            .expression =
                "if var == 5:\n"
                "    new_var = 1\n"
                "    print(\"new_var is 1\")\n"
                "\n"
                "if var == 6:\n"
                "    new_var = 2\n"
                "    print(\"new_var is 2\")",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementIf,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::Equality,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 1 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                    std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 1" } )
                                )
                            )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::StatementIf,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::Equality,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 6 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 2 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                    std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 2" } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "if var == 5:\n"
                "    new_var = 1\n"
                "    print(\"new_var is 1\")\n"
                "else:\n"
                "    new_var = 2\n"
                "    print(\"new_var is 2\")",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementIf,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::Equality,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 1 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                    std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 1" } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementElse,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Assign,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 2 } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Call,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                            std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 2" } )
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
            .expression =
                "if var == 5:\n"
                "    new_var = 1\n"
                "    print(\"new_var is 1\")\n"
                "elif var == 6:\n"
                "    new_var = 2\n"
                "    print(\"new_var is 2\")\n"
                "elif var == 7:\n"
                "    new_var = 3\n"
                "    print(\"new_var is 3\")\n"
                "else:\n"
                "    new_var = 4\n"
                "    print(\"new_var is 4\")",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementIf,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::Equality,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 1 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                    std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 1" } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementElif,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Equality,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 6 } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Assign,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 2 } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Call,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                            std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 2" } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::StatementElif,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::Equality,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 7 } )
                                                )
                                            ),
                                            std::make_unique< Node >
                                            (
                                                NodeType::Assign,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 3 } )
                                                )
                                            ),
                                            std::make_unique< Node >
                                            (
                                                NodeType::Call,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                                    std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 3" } )
                                                )
                                            ),
                                            std::make_unique< Node >
                                            (
                                                NodeType::StatementElse,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >
                                                    (
                                                        NodeType::Assign,
                                                        makeChildren
                                                        (
                                                            std::make_unique< Node >( A1::Identifier{ .name = "new_var" } ),
                                                            std::make_unique< Node >( A1::Number{ .value = 4 } )
                                                        )
                                                    ),
                                                    std::make_unique< Node >
                                                    (
                                                        NodeType::Call,
                                                        makeChildren
                                                        (
                                                            std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                                            std::make_unique< Node >( A1::StringLiteral{ .value = "new_var is 4" } )
                                                        )
                                                    )
                                                )
                                            )
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
            .expression =
                "while i < 5:\n"
                "    i = i + 1",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementWhile,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::LessThan,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 1 } )
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
            .expression =
                "while i < 5:\n"
                "    print(i)\n"
                "    i = i + 1",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::StatementWhile,
                        makeChildren
                        (
                            std::make_unique< Node >
                            (
                                NodeType::LessThan,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                                    std::make_unique< Node >( A1::Identifier{ .name = "i"     } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "i" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 1 } )
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
            .expression =
                "def func() -> num:\n"
                "    return 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementReturn,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "def func(self):\n"
                "    self.foo = 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::MemberCall,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "self" } ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "foo"  } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "def func(self):\n"
                "    self.foo = 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::MemberCall,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "self" } ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "foo"  } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "def func(self) -> num:\n"
                "    self.balances[addr] = 5\n"
                "    return self.balances[addr]",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } )
                                )
                            ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Index,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::MemberCall,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "self"     } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "balances" } )
                                                )
                                            ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "addr" } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementReturn,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Index,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::MemberCall,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "self"     } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "balances" } )
                                                )
                                            ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "addr" } )
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
            .expression =
                "def func(param1: num, param2: num):\n"
                "    var = param1 + param2",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "param2" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "param2" } )
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
            .expression =
                "# Simple function definition\n"
                "\n"
                "def func(param1: num, param2: num) -> num:\n"
                "    return param1 + param2",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "param2" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementReturn,
                                makeChildren
                                (
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "param2" } )
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
            .expression =
                "def func(param1: num, param2: num) -> num:\n"
                "    let sum: num\n"
                "    sum = param1 + param2\n"
                "    return sum\n"
                "\n"
                "let var = func(5, 5)",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::FunctionDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionParameterDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "param2" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >
                            (
                                NodeType::VariableDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "sum" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::Assign,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "sum" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Addition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                            std::make_unique< Node >( A1::Identifier{ .name = "param2" } )
                                        )
                                    )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementReturn,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "sum" } )
                                )
                            )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } ),
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "let var1: address\n"
                "let var2: array[num]\n"
                "let var3: bool\n"
                "let var4: map[num, str]\n"
                "let var5: num\n"
                "let var6: str\n"
                "let var7: i64\n"
                "let var8: u8",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var1" } ),
                            std::make_unique< Node >( A1::Registry::getAddressHandle() )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var2" } ),
                            std::make_unique< Node >( A1::Registry::getArrayHandle( A1::Registry::getNumHandle() ) )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var3" } ),
                            std::make_unique< Node >( A1::Registry::getBoolHandle() )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var4" } ),
                            std::make_unique< Node >( A1::Registry::getMapHandle( A1::Registry::getNumHandle(), A1::Registry::getStrHandle() ) )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var5" } ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var6" } ),
                            std::make_unique< Node >( A1::Registry::getStrHandle() )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var7" } ),
                            std::make_unique< Node >( A1::Registry::getI64Handle() )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var8" } ),
                            std::make_unique< Node >( A1::Registry::getU8Handle() )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression   = "let var = 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression = "let var = get()",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "get" } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "let var1: num = 5\n"
                "let var2: num = -5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var1" } ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var2" } ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >
                            (
                                NodeType::UnaryMinus,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Number{ .value = 5 } )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "let var: num = 5\n"
                "var = 10",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::Assign,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >( A1::Number{ .value = 10 } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "class Example:\n"
                "\n"
                "\n"
                "    let member1: str\n"
                "    let member2: num",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::ClassDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "Example" } ),
                            std::make_unique< Node >
                            (
                                NodeType::VariableDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "member1" } ),
                                    std::make_unique< Node >( A1::Registry::getStrHandle() )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::VariableDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "member2" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() )
                                )
                            )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "contract Example:\n"
                "    pass\n"
                "let var: num = 5",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::ContractDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "Example" } ),
                            std::make_unique< Node >
                            (
                                NodeType::StatementPass
                            )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >( A1::Registry::getNumHandle() ),
                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                        )
                    )
                )
            )
        },
        TestParameter
        {
            .expression =
                "contract Example:\n"
                "    def func(x: num) -> num:\n"
                "        let y = 0\n"
                "        if x > 3:\n"
                "            y = x\n"
                "        elif x != 0:\n"
                "            y = 1\n"
                "        return y",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::ContractDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "Example" } ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::FunctionParameterDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "x" } ),
                                            std::make_unique< Node >( A1::Registry::getNumHandle() )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::VariableDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "y" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 0 } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::StatementIf,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::GreaterThan,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "x" } ),
                                                    std::make_unique< Node >( A1::Number{ .value = 3 } )
                                                )
                                            ),
                                            std::make_unique< Node >
                                            (
                                                NodeType::Assign,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "y" } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "x" } )
                                                )
                                            ),
                                            std::make_unique< Node >
                                            (
                                                NodeType::StatementElif,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >
                                                    (
                                                        NodeType::Inequality,
                                                        makeChildren
                                                        (
                                                            std::make_unique< Node >( A1::Identifier{ .name = "x" } ),
                                                            std::make_unique< Node >( A1::Number{ .value = 0 } )
                                                        )
                                                    ),
                                                    std::make_unique< Node >
                                                    (
                                                        NodeType::Assign,
                                                        makeChildren
                                                        (
                                                            std::make_unique< Node >( A1::Identifier{ .name = "y" } ),
                                                            std::make_unique< Node >( A1::Number{ .value = 1 } )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::StatementReturn,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "y" } )
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
            .expression =
                "contract Example:\n"
                "    let foo: num = 101\n"
                "\n"
                "    def get(self) -> num:\n"
                "        return self.foo\n"
                "\n"
                "    def set_dummy(self):\n"
                "        self.foo = 5\n"
                "\n"
                "    def func(self, param1: num, param2: num) -> num:\n"
                "        self.foo = param1 + param2\n"
                "        return self.foo\n"
                "\n"
                "let var = Example()\n"
                "print(var.foo)\n"
                "print(var.func(3, 4))",
            .expectedRoot = std::make_shared< Node >
            (
                NodeType::ModuleDefinition,
                makeChildren
                (
                    std::make_unique< Node >
                    (
                        NodeType::ContractDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "Example" } ),
                            std::make_unique< Node >
                            (
                                NodeType::VariableDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "foo" } ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() ),
                                    std::make_unique< Node >( A1::Number{ .value = 101 } )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "get" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::FunctionParameterDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "self" } )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::StatementReturn,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::MemberCall,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "foo"  } )
                                                )
                                            )
                                        )
                                    )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "set_dummy" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::FunctionParameterDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "self" } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Assign,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::MemberCall,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "foo"  } )
                                                )
                                            ),
                                            std::make_unique< Node >( A1::Number{ .value = 5 } )
                                        )
                                    )
                                )
                            ),
                            std::make_unique< Node >
                            (
                                NodeType::FunctionDefinition,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::FunctionParameterDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "self" } )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::FunctionParameterDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                            std::make_unique< Node >( A1::Registry::getNumHandle() )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::FunctionParameterDefinition,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "param2" } ),
                                            std::make_unique< Node >( A1::Registry::getNumHandle() )
                                        )
                                    ),
                                    std::make_unique< Node >( A1::Registry::getNumHandle() ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Assign,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::MemberCall,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "foo"  } )
                                                )
                                            ),
                                            std::make_unique< Node >
                                            (
                                                NodeType::Addition,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "param1" } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "param2" } )
                                                )
                                            )
                                        )
                                    ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::StatementReturn,
                                        makeChildren
                                        (
                                            std::make_unique< Node >
                                            (
                                                NodeType::MemberCall,
                                                makeChildren
                                                (
                                                    std::make_unique< Node >( A1::Identifier{ .name = "self" } ),
                                                    std::make_unique< Node >( A1::Identifier{ .name = "foo"  } )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::VariableDefinition,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                            std::make_unique< Node >
                            (
                                NodeType::Call,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "Example" } )
                                )
                            )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::Call,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                            std::make_unique< Node >
                            (
                                NodeType::MemberCall,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >( A1::Identifier{ .name = "foo" } )
                                )
                            )
                        )
                    ),
                    std::make_unique< Node >
                    (
                        NodeType::Call,
                        makeChildren
                        (
                            std::make_unique< Node >( A1::Identifier{ .name = "print" } ),
                            std::make_unique< Node >
                            (
                                NodeType::MemberCall,
                                makeChildren
                                (
                                    std::make_unique< Node >( A1::Identifier{ .name = "var" } ),
                                    std::make_unique< Node >
                                    (
                                        NodeType::Call,
                                        makeChildren
                                        (
                                            std::make_unique< Node >( A1::Identifier{ .name = "func" } ),
                                            std::make_unique< Node >( A1::Number{ .value = 3 } ),
                                            std::make_unique< Node >( A1::Number{ .value = 4 } )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
        }
    )
);

namespace
{
    struct ErrorTestParameter
    {
        std::string_view expression;
        std::string_view expectedErrorMessage;

        friend std::ostream & operator<<( std::ostream & os, ErrorTestParameter const & param )
        {
            return os << param.expression;
        }
    };

    struct ASTErrorTestFixture : ::testing::TestWithParam< ErrorTestParameter > {};
} // namespace

TEST_P( ASTErrorTestFixture, parsingError )
{
    auto const [ expression, expectedErrorMessage ]{ GetParam() };

    EXPECT_THROW
    (
        {
            try
            {
                auto token{ A1::tokenize( A1::Stream{ expression } ) };
                [[ maybe_unused ]] auto root { A1::AST::parse( token ) };
            }
            catch ( A1::ParsingError const & ex )
            {
                EXPECT_STREQ( expectedErrorMessage.data(), ex.what() );
                throw;
            }
        },
        A1::ParsingError
    );
}

INSTANTIATE_TEST_SUITE_P
(
    ASTErrorTest,
    ASTErrorTestFixture,
    ::testing::Values
    (
        ErrorTestParameter
        {
            .expression           = "let = 5",
            .expectedErrorMessage = "1:6: error: Expecting identifier"
        },
        ErrorTestParameter
        {
            .expression           = "let var: = 5",
            .expectedErrorMessage = "1:11: error: Expecting type identifier"
        },
        ErrorTestParameter
        {
            .expression           = "contract :",
            .expectedErrorMessage = "1:11: error: Expecting identifier"
        },
        ErrorTestParameter
        {
            .expression           = "var = 5 * (5 + 5",
            .expectedErrorMessage = "1:17: error: Expecting closing parenthesis"
        },
        ErrorTestParameter
        {
            .expression           = "var = 5 * (5 + )",
            .expectedErrorMessage = "1:17: error: Expecting 2 operands (1 given)"
        },
        ErrorTestParameter
        {
            .expression           = "var = ()",
            .expectedErrorMessage = "1:9: error: Expecting an expression inside parentheses"
        },
        ErrorTestParameter
        {
            .expression           = "def func(param1: num",
            .expectedErrorMessage = "1:21: error: Expecting closing parenthesis"
        },
        ErrorTestParameter
        {
            .expression           = "def func(param1: num) -> ",
            .expectedErrorMessage = "1:26: error: Expecting type identifier"
        },
        ErrorTestParameter
        {
            .expression           = "func(var1, var2",
            .expectedErrorMessage = "1:16: error: Expecting closing parenthesis"
        },
        ErrorTestParameter
        {
            .expression           = "var = 5 * 5 5",
            .expectedErrorMessage = "1:14: error: Unexpected operand"
        },
        ErrorTestParameter
        {
            .expression           = "if var == 5",
            .expectedErrorMessage = "1:12: error: Expecting ':'"
        }
    )
);