#pragma once

#include "OP.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

class Source;


class TokenMain
{
public:

	enum Type
	{
	BLANK,
	END,
	LITERAL,
	IDENTIFIER,
	STRING_LITERAL,
	OPERATION,
	COMMENT,
	MULTI_COMMENT,
	SCOPE,
	CAST,
	TUPLE,
	UNKNOWN
	};

	TokenData(string input, shared_ptr<Source> fileIn, int line_int, int char_pos, Type token_type, Operator Op=Operator(nullptr))
	{
		text=textIn;
		file=fileIn;
		line=lineIn;
		charPos=charPosIn;
		tokenType=tokenTypeIn;
		op=opIn;
	}
