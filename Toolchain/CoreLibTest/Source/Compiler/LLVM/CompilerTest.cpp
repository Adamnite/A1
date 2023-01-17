/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/AST/AST.hpp>
#include <CoreLib/Compiler/LLVM/Compiler.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <array>
#include <string>

namespace
{
    [[ nodiscard ]] std::string run( std::string_view const cmd )
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
        /** A1 programming language source code. */
        std::string_view input;

        /** Output of the compiled executable file. */
        std::string_view expectedOutput;

        friend std::ostream & operator<<( std::ostream & os, TestParameter const & param )
        {
            return os << param.input;
        }
    };

    struct LLVMCompilerTestFixture : ::testing::TestWithParam< TestParameter > {};
} // namespace

TEST_P( LLVMCompilerTestFixture, compilation )
{
    auto const [ input, expectedOutput ]{ GetParam() };

    auto tokenIt { A1::tokenize( A1::Stream{ input } ) };
    auto rootNode{ A1::AST::parse( tokenIt ) };

    static constexpr auto executableFilename{ "llvm_compiler_test" };
    A1::Compiler::Settings settings{ .executableFilename = executableFilename };

    ASSERT_TRUE( A1::LLVM::compile( std::move( settings ), rootNode ) )
        << "Compilation should have succeded";

    static auto const cmd{ fmt::format( "./{}", executableFilename ) };

    auto actualOutput{ run( cmd ) };
    if ( actualOutput.ends_with( '\n' ) )
    {
        // trim last '\n' character
        actualOutput.pop_back();
    }

    EXPECT_EQ( actualOutput, expectedOutput );
    std::remove( executableFilename );
}

INSTANTIATE_TEST_SUITE_P
(
    LLVMCompilerTest,
    LLVMCompilerTestFixture,
    ::testing::Values
    (
        TestParameter
        {
            .input = "",
            .expectedOutput = ""
        },
        TestParameter
        {
            .input =
                "# First comment line\n"
                "# Second comment line\n",
            .expectedOutput = ""
        },
        TestParameter
        {
            .input =
                "print(45)\n"
                "print(\"Hello, world!\")",
            .expectedOutput =
                "45\n"
                "Hello, world!"
        },
        TestParameter
        {
            .input =
                "let var1 = 5 + 5 + 5\n"
                "let var2 = (5 + 2)\n"
                "let var3 = (5 + 2) * 2\n"
                "let var4 = 2 * (5 + 2)\n"
                "let var5 = (5 + 2) * (5 + 2)\n"
                "print(var1)\n"
                "print(var2)\n"
                "print(var3)\n"
                "print(var4)\n"
                "print(var5)",
            .expectedOutput =
                "15\n"
                "7\n"
                "14\n"
                "14\n"
                "49"
        },
        TestParameter
        {
            .input =
                "let var1: bool = True\n"
                "let var2: num = 5\n"
                "let var3: str = \"foo\"\n"
                "let var4: i8 = -12\n"
                "let var5: u16 = 12345\n"
                "print(var1)\n"
                "print(var2)\n"
                "print(var3)\n"
                "print(var4)\n"
                "print(var5)",
            .expectedOutput =
                "1\n"
                "5\n"
                "foo\n"
                "-12\n"
                "12345"
        },
        TestParameter
        {
            .input =
                "let i: num = 0\n"
                "if i == 0:\n"
                "    print(\"Inside 1st if\")\n"
                "    print(\"i is 0\")\n"
                "\n"
                "if i == 5:\n"
                "    print(\"Inside 2nd if\")\n"
                "    print(\"i is 5\")",
            .expectedOutput =
                "Inside 1st if\n"
                "i is 0"
        },
        TestParameter
        {
            .input =
                "let i: num = 0\n"
                "let j: num = 4\n"
                "if i > j:\n"
                "    print(\"Inside if\")\n"
                "    print(i)\n"
                "else:\n"
                "    print(\"Inside else\")\n"
                "    print(j)",
            .expectedOutput =
                "Inside else\n"
                "4"
        },
        TestParameter
        {
            .input =
                "let i: num = 0\n"
                "let j: num = 4\n"
                "if i > j:\n"
                "    print(\"Inside if\")\n"
                "    print(i)\n"
                "elif i != 0:\n"
                "    print(\"Inside 1st elif\")\n"
                "    print(\"i is not 0\")\n"
                "elif j == 4:\n"
                "    print(\"Inside 2nd elif\")\n"
                "    if i == 0:\n"
                "         print(\"i is 0\")\n"
                "    print(\"j is 4\")\n"
                "else:\n"
                "    print(\"Inside else\")\n"
                "    print(j)",
            .expectedOutput =
                "Inside 2nd elif\n"
                "i is 0\n"
                "j is 4"
        },
        TestParameter
        {
            .input =
                "let i: num = 0\n"
                "while i < 4:\n"
                "    print(i)\n"
                "    i += 1"
                "    assert i < 5",
            .expectedOutput =
                "0\n"
                "1\n"
                "2\n"
                "3"
        },
        TestParameter
        {
            .input =
                "let s: str\n"
                "let n: num\n"
                "print(s)\n"
                "print(n)\n",
            .expectedOutput =
                "\n"
                "0"
        },
        TestParameter
        {
            .input =
                "let s = \"Hello, world!\"\n"
                "let n = 45\n"
                "print(s)\n"
                "print(n)",
            .expectedOutput =
                "Hello, world!\n"
                "45"
        },
        TestParameter
        {
            .input =
                "let s: str = \"Hello, world!\"\n"
                "let n: num = 45\n"
                "print(s)\n"
                "print(n)",
            .expectedOutput =
                "Hello, world!\n"
                "45"
        },
        TestParameter
        {
            .input =
                "let s: str = \"Hello, world!\"\n"
                "let n: num = 45\n"
                "s = \"Hello, folks!\"\n"
                "n = 450\n"
                "print(s)\n"
                "print(n)",
            .expectedOutput =
                "Hello, folks!\n"
                "450"
        },
        TestParameter
        {
            .input =
                "def func():\n"
                "    print(\"Hello, from inside!\")\n"
                "\n"
                "func()",
            .expectedOutput =
                "Hello, from inside!"
        },
        TestParameter
        {
            .input =
                "def func() -> num:\n"
                "    return 45\n"
                "\n"
                "print(func())",
            .expectedOutput =
                "45"
        },
        TestParameter
        {
            .input =
                "def sum(a: num, b: num) -> num:\n"
                "    print(\"Summing...\")\n"
                "    return a + b\n"
                "\n"
                "let var = sum(4, 5)\n"
                "print(var)",
            .expectedOutput =
                "Summing...\n"
                "9"
        },
        TestParameter
        {
            .input =
                "def get() -> str:\n"
                "    return \"Hello, folks!\"\n"
                "\n"
                "contract HelloWorld:\n"
                "    def get() -> str:\n"
                "        return \"Hello, world!\"\n"
                "\n"
                "let var = HelloWorld()\n"
                "print(var.get())\n"
                "print(get())",
            .expectedOutput =
                "Hello, world!\n"
                "Hello, folks!"
        },
        TestParameter
        {
            .input =
                "contract Addition:\n"
                "    let sum: num = 0\n"
                "\n"
                "    def add(self, x: num, y: num) -> num:\n"
                "        self.sum = x + y\n"
                "        return self.sum\n"
                "\n"
                "let var = Addition()\n"
                "print(var.sum)\n"
                "print(var.add(1, 2))\n"
                "print(var.sum)",
            .expectedOutput =
                "0\n"
                "3\n"
                "3"
        },
        TestParameter
        {
            .input =
                "contract Addition:\n"
                "    let sum: num = 0\n"
                "    let name: str = \"Addition\"\n"
                "\n"
                "    def __init__(self):\n"
                "        self.sum = 5\n"
                "\n"
                "    def add(self, x: num, y: num) -> num:\n"
                "        let sum = x + y\n"
                "        return sum\n"
                "\n"
                "let var = Addition()\n"
                "print(var.name)\n"
                "print(var.sum)\n"
                "print(var.add(1, 2))\n"
                "print(var.sum)",
            .expectedOutput =
                "Addition\n"
                "5\n"
                "3\n"
                "5"
        }
    )
);