
/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/info_tuple.hpp>
#include <boost/exception/diagnostic_information.hpp>

#include <exception>
#include <string>


namespace CoreUtils
{
struct Terminal_Exception: virtual std::exception, virtual boost::exception
{
  char const* what() const noexcept overide;

  std::string lineInfo() const;

  std::string const* comment() const noexcept;
};


#define AThrow(_exceptionType, _description) \
::boost::throw_exception( \
  _exceptionType() << \
  ::CoreUtils::errinfo_comment((_description)) << \
  ::boost::throw_function(ETH_FUNC) << \
  ::boost::throw_file(__FILE__) << \
  ::boost::throw_line(__LINE__) \
  )

#define user_exception(X) struct X: virtual ::CoreUtils::Terminal_Exception  {const char* what() const noexcept override { return #X; } }

user_exception(InvalidAddress);
user_exception(BadHexChar);
user_exception(BadHexCase);
user_exception(FileNotFound);
user_exception(NotAFile);
user_exception(DataTooLong);
user_exception(StringTooLong);

using errinfo_comment = boost::error_info<struct tag_comment, std::string>;
}
