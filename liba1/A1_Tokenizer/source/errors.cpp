// Copyright 2022 The Adamnite C++ Authors
//This code is open-sourced under the MIT License.

#include "errors.hpp"

namespace Adamnite {
	error::error(std::string message, size_t line_number, size_t char_index) noexcept :
		_message(std::move(message)),
		_line_number(line_number),
		_char_index(char_index)
	{
	}
		
	const char* error::what() const noexcept {
		return _message.c_str();
	}
		
	size_t error::line_number() const noexcept {
		return _line_number;
	}
	
	size_t error::char_index() const noexcept {
		return _char_index;
	}
	
	error parsing_error(const char* message, size_t line_number, size_t char_index) {
		std::string error_message("Parsing error: ");
		error_message += message;
		return error(std::move(error_message), line_number, char_index);
	}
	
	error unexpected_error(std::string_view unexpected, size_t line_number, size_t char_index) {
		std::string message("Unexpected '");
		message += unexpected;
		message += "'";
		return parsing_error(message.c_str(), line_number, char_index);
	}
	
	void format_error(const error& err, get_character source, std::ostream& output) {
		output << "(" << (err.line_number() + 1) << ") " << err.what() << std::endl;
		
		size_t char_index = 0;
		
		for (size_t line_number = 0; line_number < err.line_number(); ++char_index) {
			int c = source();
			if (c < 0) {
				return;
			} else if (c == '\n') {
				++line_number;
			}
		}

		size_t index_in_line = err.char_index() - char_index;
		
		std::string line;
		for (size_t idx = 0;; ++idx) {
			int c = source();
			if (c < 0 || c == '\n' || c == '\r') {
				break;
			}
			line += char(c);
		}
		
		output << line << std::endl;
		
		for (size_t idx = 0; idx < index_in_line; ++idx) {
			output << " ";
		}
		
		output << "^" << std::endl;
	}
};

