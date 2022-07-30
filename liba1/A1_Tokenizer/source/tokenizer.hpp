// Copyright 2022 The Adamnite C++ Authors
//This code is open-sourced under the MIT License.

#ifndef tokenizer_hpp
#define tokenizer_hpp

#include <functional>
#include <string_view>
#include <iostream>
#include <variant>

#include "tokens.hpp"
#include "push_back_stream.hpp"

namespace Adamnite {
	token tokenize(push_back_stream& stream);
}


#endif /* tokenizer_hpp */
