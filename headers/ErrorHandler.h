#pragma once

#include "Token.h"

#include <string>
using std::string;

#define FUNC string(__FUNCTION__)
//#define FUNC string(__PRETTY_FUNCTION__) //like __FUNCTION__, but adds class name

enum ErrorPriority
{
	SOURCE_ERROR,
	SOURCE_WARNING,
	JSYK,
	INTERNAL_ERROR,
	RUNTIME_ERROR
};

class ErrorHandler
{
public:

	static string priorityToStr(ErrorPriority in);

	void log(string msg, ErrorPriority priority, Token token=nullptr);

	void msg(string in);

	bool getIfErrorLogged() {return errorHasBeenLogged;}

private:

	bool errorHasBeenLogged=false;
};

extern ErrorHandler error;

class A1Error
{
public:
	A1Error(string msgIn, ErrorPriority priorityIn, Token tokenIn=nullptr);

	void log();

private:
	string msg;
	ErrorPriority priority;
	Token token;

};
