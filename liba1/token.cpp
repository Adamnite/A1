// Copyright 2022 The Adamnite C++ Authors
//This code is open-sourced under the MIT License. 


#include "token.hpp"
#include <map>

using namespace std;

namespace dev
{
    namespace Adamnite
    {

    //For Name
        #define adamTok(name, string, precedence) #name,
        char const* const Token::m_name[NUM_TOKENS] =
        {
            TOKEN_LIST(adamTok, adamTok)
        };
        #undef adamTok

    //For String
        #define adamTok(name, string, precedence) string,
        char const* const Token::m_string[NUM_TOKENS] =
        {
            TOKEN_LIST(adamTok, adamTok)
        };
        #undef adamTok

    // For Precedence

        #define adamTok(name, string, precedence) precedence,
        int8_t const Token::m_precedence[NUM_TOKENS] =
        {
            TOKEN_LIST(adamTok, adamTok)
        };
        #undef adamTok


        #define adamKT(a, b, c) 'T',
        #define adamKK(a, b, c) 'K',
        char const Token::m_tokenType[] =
        {
            TOKEN_LIST(adamKT, adamKK)
        };
        Token::Value Token::fromIdentifierOrKeyword(const std::string& _name)
        {
            // The following macros are used inside TOKEN_LIST and cause non-keyword tokens to be ignored
            // and keywords to be put inside the keywords variable.
            #define KEYWORD(name, string, precedence) {string, Token::name},
            #define TOKEN(name, string, precedence)
            static const map<string, Token::Value> keywords({TOKEN_LIST(TOKEN, KEYWORD)});
            #undef KEYWORD
            #undef TOKEN
            auto it = keywords.find(_name);
            return it == keywords.end() ? Token::Identifier : it->second;
        }
        #undef adamKT
        #undef adamKK
    }
}
