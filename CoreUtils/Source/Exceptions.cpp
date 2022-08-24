
/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */
#include <Exceptions.hpp>

using namespace std;
using namespace CoreUtils;

char const* Terminal_Exception::what() const noexcept
{
	// Return the comment if available.
	if (string const* cmt = comment())
		return cmt->data();

	// Fallback to base what().
	// Boost accepts nullptr, but the C++ standard doesn't
	// and crashes on some platforms.
	return std::Terminal_Exception::what();
}

string Terminal_Exception::lineInfo() const
{
	char const* const* file = boost::get_error_info<boost::throw_file>(*this);
	int const* line = boost::get_error_info<boost::throw_line>(*this);
	string ret;
	if (file)
		ret += *file;
	ret += ':';
	if (line)
		ret += to_string(*line);
	return ret;
}

string const* Terminal_Exception::comment() const noexcept
{
	return boost::get_error_info<errinfo_comment>(*this);
}
