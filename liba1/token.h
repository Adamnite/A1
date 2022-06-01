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
// are signed as follows: (token_name,source_code, precendence). The name is the name of the
//token within the protocol, the string within the source code, and the precedence.

//We also use IGNORE_TOKEN and keywords, as with Solidity (please change)
//      T are non-keyword token
#define IGNORE_TOKEN(token_name, source_code, precedence)

#define TOKEN_LIST(T,K)
        //End of source declration
        T(END, "EOS", 0)
\
        //Basic Puncuation
        T(LPAR, "(", 0)
        T(RPAR, ")", 0)
        T(LSQB, "[", 0)
        T(RSQB, "]", 0)
        T(LBR, "{", 0)
        T(RBR, "}", 0)
        T(COLON, ":", 0)
        T(SEMI, ";", 0)
        T(DOT, ".", 0)
        
