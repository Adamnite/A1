
/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */
 
#pragma once

#include <string>

namespace CoreUtils
{

enum class PanicCode
{
	Generic = 0x00, // generic / unspecified error
	Assert = 0x01, // used by the assert() builtin
	UnderOverflow = 0x11, // arithmetic underflow or overflow
	DivisionByZero = 0x12, // division or modulo by zero
	EnumConversionError = 0x21, // enum conversion error
	StorageEncodingError = 0x22, // invalid encoding in storage
	EmptyArrayPop = 0x31, // empty array pop
	ArrayOutOfBounds = 0x32, // array out of bounds access
	ResourceError = 0x41, // resource error (too large allocation or too large array)
	InvalidInternalFunction = 0x51, // calling invalid internal function
	InvalidImport = 0x61,
	InvalidType = 0x71,
};

}
