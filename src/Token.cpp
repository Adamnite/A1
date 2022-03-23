#include "../headers/Token.h"
#include "../headers/string_functions.h"
#include "../headers/utils/stringUtils.h"

Token makeToken(string textIn, shared_ptr<SourceFile> file, int line, int charPos, TokenData::Type tokenTypeIn, Operator opIn )
{
  if (str::hasprefix(textIn, "\") && !str::hasSuffix(textIn, "\"))
      textIn += "\";
  return Token(new TokenData(textIn, file, charPos, tokenType, op));

}
Token makeToken(string textIn)
{
	return Token(new TokenData(textIn, nullptr, 0, 0, TokenData::IDENTIFIER, Operator(nullptr)));
}

string TokenData::typeToString(TokenData::Type in)
{
	switch (in)
	{
		case WHITESPACE: return "whitespace";
		case LINE_END: return "newline";
		case IDENTIFIER: return "identifier";
		case LITERAL: return "literal";
		case STRING_LITERAL: return "string literal";
		case OPERATOR: return "operator";
		case LINE_COMMENT: return "single line comment";
		case BLOCK_COMMENT: return "block comment";
		case SCOPE: return "scope";
		case UNKNOWN: return "Defined type unknown";
		default: return "ERROR GETTING TOKEN TYPE";
	}
}

string TokenData::getDescription()
{
	return to_string(getLine()) + ":" + to_string(getCharPos()) + " (" + TokenData::typeToString(tokenType) + " '" + getText() + "')";
}

string TokenData::getTypeDescription()
{
	string out;

	if (tokenType==TokenData::OPERATOR)
	{
		if (op)
		{
			out+=op->getText()+" ";
		}
		else
		{
			out+="unknown ";
		}
	}

	out+=TokenData::typeToString(tokenType);

	return out;
}

string tableStringFromTokens(const vector<Token>& tokens, string tableName)
{
	/*string out="Name\tType\n";

	for (unsigned i=0; i<tokens.size(); ++i)
	{
		out+=replaceSubstring(tokens[i]->getText(), "\n", "\\n") + "\t";

		out+=tokens[i]->getTypeDescription();

		if (i<tokens.size()-1)
			out+="\n";
	}

	return putStringInTable(out, tableName);*/

	vector<string> lines;
	string above="", below="";
	string str="";
	//const int tabSize=1;
	const int maxWidth = 240;
	const string spacer = "    ";
	//const string spacer="   |   ";

	for (unsigned i=0; i<tokens.size(); ++i)
	{
		//string next=" | ";

		//for (int j=0; j<int(tabSize-str.size()%tabSize); j++)
		//	next+=" ";

		//next+=tokens[i]->getText();

		if (i>0 && str.size()+spacer.size()+tokens[i]->getText().size()<maxWidth)
		{
			above+=spacer;
			str+=spacer;
			below+=spacer;

			for (unsigned j=0; j<tokens[i]->getText().size(); j++)
			{
				above+=" ";
				below+=" ";
			}

			str+=tokens[i]->getText();
		}
		else
		{
			if (i>0)
			{
				lines.push_back(above);
				lines.push_back(str);
				lines.push_back(below);
				lines.push_back("");
				above="";
				below="";
			}

			for (unsigned j=0; j<tokens[i]->getText().size(); j++)
			{
				above+=" ";
				below+=" ";
			}

			str=tokens[i]->getText();
		}
	}

	lines.push_back(above);
	lines.push_back(str);
	lines.push_back(below);

	return lineListToBoxedString(lines, tableName, -1, false, maxWidth+4);
}

string stringFromTokens(const vector<Token>& tokens, int left, int right)
{
	string out;

	for (int i=left; i<=right; ++i)
	{
		out+=tokens[i]->getText();
		if (i<right)
			out+=" ";
	}

	return out;
}
