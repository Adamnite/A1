#pragma once
#include <iostream>
#include <string>

namespace CoreUtils{

bool isUTFencoding(std::string const& _input, size_t& _position);

///Inline bool for some form of recursiveness

inline bool isUTFencoding(std::string const& _input)
{
    size_t _position;
    return isUTFencoding(_input, _position);
}
}