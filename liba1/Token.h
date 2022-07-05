
//Copyright Adamnite Authors. See License for more info.

//Initial version of codebase inspired by Solidity. Authors reserve the right to remove this declaration once
#pragma once

//Including some Solidity's LibDevCore Library for now; change in the future
#include <libdevcore/Common.h>
#include <libdevcore/Log.h>
#include <A1/Utils.h>
#include <A1/Exceptions.h>
#include <libdevcore/UndefMacros.h>


namespace development
{
namespace A1
{
// The token list is a list of macros that define different tokens based
//on their name, string, and precedence. This is similar to the C++ implementation
//in Solidity for now (make this original later). The 3 macros are, like Solidity
// are signed as follows: (token_name,source_code, precedence). The name is the name of the
//token within the protocol, the string within the source code, and the precedence.

//We also use IGNORE_TOKEN and keywords, as with Solidity (please change)
//      T are non-keyword token
#define IGNORE_TOKEN(token_name, source_code, precedence)

#define TOKEN_LIST(T,K)
        //End of source declaration
        T(END, "EOS", 0)
\
        //Basic Punctuation
        T(LPAR, "(", 0)
        T(RPAR, ")", 0)
        T(LSQB, "[", 0)
        T(RSQB, "]", 0)
        T(LBR, "{", 0)
        T(RBR, "}", 0)
        T(COLON, ":", 0)
        T(SEMI, ";", 0)
        T(DOT, ".", 0)
        T(ARROW, "=>", 0)

\\
      //Assignment Operators
        T(ASSIGN, "=", 2)
        T(ADD_ASSIGN, "+=", 2)
        T(SUBTRACT_ASSIGN, "-=",2)
        T(MULTIPLY_ASSIGN, "*=",2)
        T(ASSIGNBITOR, "|=", 2)                                           \
        T(ASSIGNBITXOR, "^=", 2)                                          \
        T(ASSIGNBITAND, "&=", 2)                                          \
        T(AssignShl, "<<=", 2)                                            \
        T(AssignSar, ">>=", 2)                                            \
        T(AssignShr, ">>>=", 2)                                           \
        T(AssignAdd, "+=", 2)                                             \
        T(AssignSub, "-=", 2)                                             \
        T(AssignMul, "*=", 2)                                             \
        T(AssignDiv, "/=", 2)                                             \
        T(AssignMod, "%=", 2)                                             \
        \

                /* Binary operators sorted by precedence. */                       \
        /* IsBinaryOp() relies on this block of enum values */             \
        /* being contiguous and sorted in the same order! */               \
        T(Comma, ",", 1)                                                   \
        T(Or, "||", 4)                                                     \
        T(And, "&&", 5)                                                    \
        T(BitOr, "|", 8)                                                   \
        T(BitXor, "^", 9)                                                  \
        T(BitAnd, "&", 10)                                                 \
        T(SHL, "<<", 11)                                                   \
        T(SAR, ">>", 11)                                                   \
        T(SHR, ">>>", 11)                                                  \
        T(Add, "+", 12)                                                    \
        T(Sub, "-", 12)                                                    \
        T(Mul, "*", 13)                                                    \
        T(Div, "/", 13)                                                    \
        T(Mod, "%", 13)                                                    \
        T(Exp, "**", 14)                                                   \
        \
        /* Compare operators sorted by precedence. */                      \
        /* IsCompareOp() relies on this block of enum values */            \
        /* being contiguous and sorted in the same order! */               \
        T(Equal, "==", 6)                                                  \
        T(NotEqual, "!=", 6)                                               \
        T(LessThan, "<", 7)                                                \
        T(GreaterThan, ">", 7)                                             \
        T(LessThanOrEqual, "<=", 7)                                        \
        T(GreaterThanOrEqual, ">=", 7)                                     \
        K(In, "in", 7)                                                     \
        \
        /* Unary operators. */                                             \
        /* IsUnaryOp() relies on this block of enum values */              \
        /* being contiguous and sorted in the same order! */               \
        T(Not, "!", 0)                                                     \
        T(BitNot, "~", 0)                                                  \
        T(Inc, "++", 0)                                                    \
        T(Dec, "--", 0)                                                    \
        K(Delete, "delete", 0)                                             \
        \
        //Keywords


        K(FALSE, "False", 5)
        K(NONE, "None", 1)
        K(TRUE, "True", 5)
        K(AND, "and", 5)
        K(AS, "as", 3)
        K(ASSERT, "assert", 3)
        K(BREAK, "break", 8)
        K(CLASS, "Class", 6)
        K(ASYNC, "async", 4)
        K(AWAIT, "await", 4)
        K(CONTINUE, "continue", 3)
        K(DEF, "def", 7)
        K(DEL, "del", 4)
        K(ELIF, "elif", 5)
        K(IF, "if", 5)
        K(IMPORT "import", 6)
        K(ELSE, "else", 5)
        K(CLASS, "class", 8)
        K(WHILE, "while", 7)
        K(RETURN, "return", 7)
        K(WITH, "with", 6)
        K(EXCEPT, "except", 4)
        /* Adamnite subdenominations */                                       \
        K(SubMicalli, "micalli", 0)                                                \
        K(SubSunny, "sunny", 0)                                            \
        K(SubVitalik, "vitalik", 0)                                          \
        K(SubNite, "nite", 0)                                            \
        K(SubSecond, "seconds", 0)                                         \
        K(SubMinute, "minutes", 0)                                         \
        K(SubHour, "hours", 0)                                             \
        K(SubDay, "days", 0)                                               \
        K(SubWeek, "weeks", 0)                                             \
        K(SubYear, "years", 0)                                             \
        K(After, "after", 0)                                               \
        /* type keywords, keep them in this order, keep int as first keyword
         * the implementation in Types.cpp has to be synced to this here */\
        K(Int, "int", 0)                                                   \
        K(Int8, "int8", 0)                                                 \
        K(Int16, "int16", 0)                                               \
        K(Int24, "int24", 0)                                               \
        K(Int32, "int32", 0)                                               \
        K(Int40, "int40", 0)                                               \
        K(Int48, "int48", 0)                                               \
        K(Int56, "int56", 0)                                               \
        K(Int64, "int64", 0)                                               \
        K(Int72, "int72", 0)                                               \
        K(Int80, "int80", 0)                                               \
        K(Int88, "int88", 0)                                               \
        K(Int96, "int96", 0)                                               \
        K(Int104, "int104", 0)                                             \
        K(Int112, "int112", 0)                                             \
        K(Int120, "int120", 0)                                             \
        K(Int128, "int128", 0)                                             \
        K(Int136, "int136", 0)                                             \
        K(Int144, "int144", 0)                                             \
        K(Int152, "int152", 0)                                             \
        K(Int160, "int160", 0)                                             \
        K(Int168, "int168", 0)                                             \
        K(Int176, "int178", 0)                                             \
        K(Int184, "int184", 0)                                             \
        K(Int192, "int192", 0)                                             \
        K(Int200, "int200", 0)                                             \
        K(Int208, "int208", 0)                                             \
        K(Int216, "int216", 0)                                             \
        K(Int224, "int224", 0)                                             \
        K(Int232, "int232", 0)                                             \
        K(Int240, "int240", 0)                                             \
        K(Int248, "int248", 0)                                             \
        K(Int256, "int256", 0)                                             \
        K(UInt, "uint", 0)                                                 \
        K(UInt8, "uint8", 0)                                               \
        K(UInt16, "uint16", 0)                                             \
        K(UInt24, "uint24", 0)                                             \
        K(UInt32, "uint32", 0)                                             \
        K(UInt40, "uint40", 0)                                             \
        K(UInt48, "uint48", 0)                                             \
        K(UInt56, "uint56", 0)                                             \
        K(UInt64, "uint64", 0)                                             \
        K(UInt72, "uint72", 0)                                             \
        K(UInt80, "uint80", 0)                                             \
        K(UInt88, "uint88", 0)                                             \
        K(UInt96, "uint96", 0)                                             \
        K(UInt104, "uint104", 0)                                           \
        K(UInt112, "uint112", 0)                                           \
        K(UInt120, "uint120", 0)                                           \
        K(UInt128, "uint128", 0)                                           \
        K(UInt136, "uint136", 0)                                           \
        K(UInt144, "uint144", 0)                                           \
        K(UInt152, "uint152", 0)                                           \
        K(UInt160, "uint160", 0)                                           \
        K(UInt168, "uint168", 0)                                           \
        K(UInt176, "uint178", 0)                                           \
        K(UInt184, "uint184", 0)                                           \
        K(UInt192, "uint192", 0)                                           \
        K(UInt200, "uint200", 0)                                           \
        K(UInt208, "uint208", 0)                                           \
        K(UInt216, "uint216", 0)                                           \
        K(UInt224, "uint224", 0)                                           \
        K(UInt232, "uint232", 0)                                           \
        K(UInt240, "uint240", 0)                                           \
        K(UInt248, "uint248", 0)                                           \
        K(UInt256, "uint256", 0)                                           \
        K(Bytes1, "bytes1", 0)                                             \
        K(Bytes2, "bytes2", 0)                                             \
        K(Bytes3, "bytes3", 0)                                             \
        K(Bytes4, "bytes4", 0)                                             \
        K(Bytes5, "bytes5", 0)                                             \
        K(Bytes6, "bytes6", 0)                                             \
        K(Bytes7, "bytes7", 0)                                             \
        K(Bytes8, "bytes8", 0)                                             \
        K(Bytes9, "bytes9", 0)                                             \
        K(Bytes10, "bytes10", 0)                                           \
        K(Bytes11, "bytes11", 0)                                           \
        K(Bytes12, "bytes12", 0)                                           \
        K(Bytes13, "bytes13", 0)                                           \
        K(Bytes14, "bytes14", 0)                                           \
        K(Bytes15, "bytes15", 0)                                           \
        K(Bytes16, "bytes16", 0)                                           \
        K(Bytes17, "bytes17", 0)                                           \
        K(Bytes18, "bytes18", 0)                                           \
        K(Bytes19, "bytes19", 0)                                           \
        K(Bytes20, "bytes20", 0)                                           \
        K(Bytes21, "bytes21", 0)                                           \
        K(Bytes22, "bytes22", 0)                                           \
        K(Bytes23, "bytes23", 0)                                           \
        K(Bytes24, "bytes24", 0)                                           \
        K(Bytes25, "bytes25", 0)                                           \
        K(Bytes26, "bytes26", 0)                                           \
        K(Bytes27, "bytes27", 0)                                           \
        K(Bytes28, "bytes28", 0)                                           \
        K(Bytes29, "bytes29", 0)                                           \
        K(Bytes30, "bytes30", 0)                                           \
        K(Bytes31, "bytes31", 0)                                           \
        K(Bytes32, "bytes32", 0)                                           \
        K(Bytes, "bytes", 0)                                               \
        K(Byte, "byte", 0)                                                 \
        K(String, "string", 0)                                             \
        K(Address, "address", 0)                                           \
        K(Bool, "bool", 0)                                                 \
        K(Real, "real", 0)                                                 \
        K(UReal, "ureal", 0)                                               \
        T(TypesEnd, NULL, 0) /* used as type enum end marker */            \
        \
        /* Literals */                                                     \
        K(NullLiteral, "null", 0)                                          \
        K(TrueLiteral, "true", 0)                                          \
        K(FalseLiteral, "false", 0)                                        \
        T(Number, NULL, 0)                                                 \
        T(StringLiteral, NULL, 0)                                          \
        T(CommentLiteral, NULL, 0)                                         \
        \
        /* Identifiers (not keywords or future reserved words). */         \
        T(Identifier, NULL, 0)                                             \
        \
        /* Keywords reserved for future. use. */                           \
        K(As, "as", 0)                                                     \
        K(Case, "case", 0)                                                 \
        K(Catch, "catch", 0)                                               \
        K(Final, "final", 0)                                               \
        K(Let, "let", 0)                                                   \
        K(Match, "match", 0)                                               \
        K(Of, "of", 0)                                                     \
        K(Relocatable, "relocatable", 0)                                   \
        K(Switch, "switch", 0)                                             \
        K(Throw, "throw", 0)                                               \
        K(Try, "try", 0)                                                   \
        K(Type, "type", 0)                                                 \
        K(TypeOf, "typeof", 0)                                             \
        K(Using, "using", 0)                                               \
        /* Illegal token - not able to scan. */                            \
        T(Illegal, "ILLEGAL", 0)                                           \
        \
        /* Scanner-internal use only. */                                   \
        T(Whitespace, NULL, 0)




//Implement basic tokenization function
//Right now, borrowed from Solidity's code. Make unique later.

class Token
{
public:
  #define T(name, string, precedence) name,
	enum Value
	{
		TOKEN_LIST(T, T)
		NUM_TOKENS
	};
#undef T

static char const* getName(Value tok)
	{
		solAssert(tok < NUM_TOKENS, "");
		return m_name[tok];
	}
  static bool isElementary(Value tok) {return Int <= tok && tok < TypesEnd; }
  static bool isAssignmentOp(Value tok) {return ASSIGN <= tok && tok <= AssignMod;}
  static bool isCommutativeOp(Value op) { return op == BitOr || op == BitXor || op == BitAnd ||
      op == ADD || op == Mul || op == Equal || op == NotEqual; }
  static bool isArithmeticOp(Value op) { return Add <= op && op <= Exp; }
  static bool isCompareOp(Value op) { return Equal <= op && op <= In; }

  static Value AssignmentToBinaryOp(Value op)
  {
    solAssert(isAssignmentOp(op) && op != Assign, "");
    return Value(op + (BitOr - AssignBitOr));
  }

  static bool isBitOp(Value op) { return (BitOr <= op && op <= SHR) || op == BitNot; }
  static bool isBooleanOp(Value op) { return (Or <= op && op <= And) || op == Not; }
  static bool isUnaryOp(Value op) { return (Not <= op && op <= Delete) || op == Add || op == Sub || op == After; }
  static bool isCountOp(Value op) { return op == Inc || op == Dec; }
  static bool isShiftOp(Value op) { return (SHL <= op) && (op <= SHR); }
  static bool isVisibilitySpecifier(Value op) { return isVariableVisibilitySpecifier(op) || op == External; }
  static bool isVariableVisibilitySpecifier(Value op) { return op == Public || op == Private || op == Internal; }
  static bool isLocationSpecifier(Value op) { return op == Memory || op == Storage; }
  static bool isEtherSubdenomination(Value op) { return op == SubWei || op == SubSzabo || op == SubFinney || op == SubEther; }
  static bool isTimeSubdenomination(Value op) { return op == SubSecond || op == SubMinute || op == SubHour || op == SubDay || op == SubWeek || op == SubYear; }

  // Returns a string corresponding to the JS token string
  // (.e., "<" for the token LT) or NULL if the token doesn't
  // have a (unique) string (e.g. an IDENTIFIER).
  static char const* toString(Value tok)
  {
    solAssert(tok < NUM_TOKENS, "");
    return m_string[tok];
  }

  // Returns the precedence > 0 for binary and compare
  // operators; returns 0 otherwise.
  static int precedence(Value tok)
  {
    solAssert(tok < NUM_TOKENS, "");
    return m_precedence[tok];
  }

  static Token::Value fromIdentifierOrKeyword(std::string const& _name);

  private:
  static char const* const m_name[NUM_TOKENS];
  static char const* const m_string[NUM_TOKENS];
  static int8_t const m_precedence[NUM_TOKENS];
  static char const m_tokenType[NUM_TOKENS];
};

}
}
