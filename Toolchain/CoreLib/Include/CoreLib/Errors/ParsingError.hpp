/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Errors/ErrorInfo.hpp>

#include <fmt/format.h>

#include <stdexcept>

namespace A1
{

struct ParsingError : std::runtime_error
{
    explicit ParsingError( ErrorInfo errorInfo, std::string_view const additionalMsg ) : std::runtime_error( "" )
    {
        msg_ = fmt::format
        (
            "{}:{}: error: {}",
            errorInfo.lineNumber,
            errorInfo.columnNumber,
            additionalMsg
        );
    }

    char const * what() const noexcept override { return msg_.data(); }

private:
    std::string msg_;
};

} // namespace A1::Compiler