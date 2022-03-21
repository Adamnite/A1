
#pragma once

#include "VERSION.h"

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <list>

using std::max;
using std::min;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::list;

#include "msclStringFuncs.h"
#include "Namespace.h"
#include "Frame.h"
#include "Token.h"
#include "Ast.h"
#include "Source.h"

extern vector<string> cmdArgs;

class Element;

class A1Program
{
public:

	A1Program();
	~A1rogram() {cleanUp();}

	string getCPP();

	void resolveProgram(string inFilename, bool printExtraOutput);

	Namespace getGlobalActionTable();

	void execute();

private:

	void cleanUp();

private:

	shared_ptr<SourceFile> file=nullptr;

	//a list of all the tokens in the program
	vector<Token> tokens;

	//root of the abstract syntax tree
	AstNode astRoot=nullptr;

	//root of the action tree
	Action actionRoot=createNewVoidAction();

	vector<char> whitespaceChars, letterChars, digitChars;
	char singleLineComment;
};
