/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/ReservedToken.hpp>

#include <algorithm>
#include <utility>
#include <array>

namespace A1
{

namespace
{
    using StringifiedToken = std::pair< std::string_view, ReservedToken >;

    constexpr std::array keywords
    {
        StringifiedToken{ "False"   , ReservedToken::False    },
        StringifiedToken{ "None"    , ReservedToken::None     },
        StringifiedToken{ "True"    , ReservedToken::True     },
        StringifiedToken{ "and"     , ReservedToken::And      },
        StringifiedToken{ "as"      , ReservedToken::As       },
        StringifiedToken{ "assert"  , ReservedToken::Assert   },
        StringifiedToken{ "break"   , ReservedToken::Break    },
        StringifiedToken{ "class"   , ReservedToken::Class    },
        StringifiedToken{ "continue", ReservedToken::Continue },
        StringifiedToken{ "def"     , ReservedToken::Def      },
        StringifiedToken{ "del"     , ReservedToken::Del      },
        StringifiedToken{ "elif"    , ReservedToken::Elif     },
        StringifiedToken{ "else"    , ReservedToken::Else     },
        StringifiedToken{ "except"  , ReservedToken::Except   },
        StringifiedToken{ "finally" , ReservedToken::Finally  },
        StringifiedToken{ "for"     , ReservedToken::For      },
        StringifiedToken{ "from"    , ReservedToken::From     },
        StringifiedToken{ "global"  , ReservedToken::Global   },
        StringifiedToken{ "if"      , ReservedToken::If       },
        StringifiedToken{ "import"  , ReservedToken::Import   },
        StringifiedToken{ "in"      , ReservedToken::In       },
        StringifiedToken{ "is"      , ReservedToken::Is       },
        StringifiedToken{ "lambda"  , ReservedToken::Lambda   },
        StringifiedToken{ "non"     , ReservedToken::Non      },
        StringifiedToken{ "nonlocal", ReservedToken::NonLocal },
        StringifiedToken{ "not"     , ReservedToken::Not      },
        StringifiedToken{ "or"      , ReservedToken::Or       },
        StringifiedToken{ "pass"    , ReservedToken::Pass     },
        StringifiedToken{ "raise"   , ReservedToken::Raise    },
        StringifiedToken{ "return"  , ReservedToken::Return   },
        StringifiedToken{ "try"     , ReservedToken::Try      },
        StringifiedToken{ "while"   , ReservedToken::While    },
        StringifiedToken{ "with"    , ReservedToken::With     },
        StringifiedToken{ "yield"   , ReservedToken::Yield    }
    };

    static_assert( std::is_sorted( std::begin( keywords ), std::end( keywords ) ) );
} // namespace

ReservedToken getKeyword(std::string_view const word) noexcept
{
    auto const it
    {
        std::find_if
        (
            std::begin( keywords ), std::end( keywords ),
            [ word ]( auto const & keyword ) noexcept
            {
                return keyword.first == word;
            }
        )
    };
    return it == std::end( keywords )
        ? ReservedToken::Unknown
        : it->second;
}

} // namespace A1