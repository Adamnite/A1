// Copyright 2022 The Adamnite C++ Authors
//This code is open-sourced under the MIT License.

#ifndef helpers_h
#define helpers_h

namespace Adamnite {

	//blatantly stolen from https://en.cppreference.com/w/cpp/utility/variant/visit
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}

#endif /* helpers_h */
