/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/LLVM/Compiler.hpp>
#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <array>
#include <future>
#include <string>

namespace
{
    std::string run( std::string_view const cmd )
    {
        std::array< char, 1048576U > buffer;
        std::string result;

#ifdef _WIN32
#   define popen  _popen
#   define pclose _pclose
#endif // _WIN32
        using FilePtr = std::unique_ptr< std::FILE, decltype( &pclose ) >;

        FilePtr pipe{ popen( cmd.data(), "r" ), &pclose };
        if ( pipe == nullptr )
        {
            throw std::runtime_error( "popen() failed!" );
        }

        try
        {
            std::size_t bytesCount{ 0 };
            while ( ( bytesCount = std::fread( buffer.data(), sizeof( char ), std::size( buffer ), pipe.get() ) ) != 0 )
            {
                result += std::string{ buffer.data(), bytesCount };
            }
        }
        catch ( ... ) { throw; }
        return result;
    }

    struct TestParameter
    {
        std::string_view title;

        /**
         * A1 programming language source code.
         */
        std::string_view input;

        /**
         * Output of the compiled executable file.
         */
        std::string_view expectedOutput;

        friend std::ostream & operator<<( std::ostream & os, TestParameter const & param )
        {
            return os << param.input;
        }
    };

    struct LLVMCompilerTestFixture : ::testing::TestWithParam< TestParameter >
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

TEST_P( LLVMCompilerTestFixture, compilation )
{
    using namespace std::chrono_literals;
    static constexpr auto timeout{ 2s };

    auto const [ _, input, expectedOutput ]{ GetParam() };

    auto tokenIt{ A1::tokenize( A1::PushBackStream{ input } ) };
    auto rootNode{ A1::parse( tokenIt, 0, false ) };

    static constexpr auto executableFilename{ "llvm_compiler_test" };
    A1::Compiler::Settings settings{ .executableFilename = executableFilename };

    ASSERT_TRUE( A1::LLVM::compile( std::move( settings ), rootNode ) )
        << "Compilation should have succeded";

    static auto const runCommand{ fmt::format( "./{}", executableFilename ) };
    auto result{ std::async ( std::launch::async, run, runCommand ) };
    auto const status{ result.wait_for( timeout ) };

    if ( status == std::future_status::timeout )
    {
        ASSERT_FALSE( true ) << "Timeout has exceeded";
    }
    else if ( status == std::future_status::ready )
    {
        auto actualOutput{ result.get() };
        if ( actualOutput.ends_with( '\n' ) )
        {
            // trim last '\n' character
            actualOutput.pop_back();
        }

        EXPECT_EQ( actualOutput, expectedOutput );
    }
}

INSTANTIATE_TEST_SUITE_P
(
    LLVMCompilerTest,
    LLVMCompilerTestFixture,
    ::testing::Values
    (
        TestParameter
        {
            .title = "Empty",
            .input = "",
            .expectedOutput = ""
        },
        TestParameter
        {
            .title = "CommentsOnly",
            .input =
                "# First comment line\n"
                "# Second comment line\n",
            .expectedOutput = ""
        },
        TestParameter
        {
            .title = "Print",
            .input =
                "print(45)\n"
                "print(\"Hello, world!\")",
            .expectedOutput =
                "45.000000\n"
                "Hello, world!"
        },
        TestParameter
        {
            .title = "VariableDefinitionWithoutInitialization",
            .input =
                "let s: str\n"
                "let n: num\n"
                "print(s)\n"
                "print(n)\n",
            .expectedOutput =
                "\n"
                "0.000000"
        },
        TestParameter
        {
            .title = "VariableDefinitionWithoutType",
            .input =
                "let s = \"Hello, world!\"\n"
                "let n = 45\n"
                "print(s)\n"
                "print(n)",
            .expectedOutput =
                "Hello, world!\n"
                "45.000000"
        },
        TestParameter
        {
            .title = "VariableDefinition",
            .input =
                "let s: str = \"Hello, world!\"\n"
                "let n: num = 45\n"
                "print(s)\n"
                "print(n)",
            .expectedOutput =
                "Hello, world!\n"
                "45.000000"
        },
        TestParameter
        {
            .title = "VariableAssignment",
            .input =
                "let s: str = \"Hello, world!\"\n"
                "let n: num = 45\n"
                "s = \"Hello, folks!\"\n"
                "n = 450\n"
                "print(s)\n"
                "print(n)",
            .expectedOutput =
                "Hello, folks!\n"
                "450.000000"
        }
    ),
    LLVMCompilerTestFixture::PrintTitle()
);