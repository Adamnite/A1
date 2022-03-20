#pragma once


#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;


#include "OP.h"

class AllOP{
public:

  static void init();

  void get(string text, vector <OP>& out);

  #define CALL_OP(operation, op_text, precedence, input, isoverload)\
          const OP name{new OpData(text, prece, input, isoverload)};
                                                                        \
  #define OP_LIST
  CALL_OP(forLoop, "for", 8, OPDATA::BOTH, false);
  CALL_OP(whileLoop, "while", 8, OPDATA::BOTH, false);          \
  CALL_OP(if, "if", 12, OPDATA::BOTH, false);	\
	CALL_OP(colon,":",22,	OPDATA::BOTH,	false);	\
	CALL_OP(doubleColon,"::",	25,	OPDATA::BOTH,	false	);	\
	CALL_OP(comma,"," , 28, OPDATA::BOTH, false);	\
	CALL_OP(orOp,	"or",	40,	OPDATA::BOTH,	false);	\
	CALL_OP(andOp,"and",38,	OPDATA::BOTH, false);	\
	CALL_OP(assign,"=", 40, OPDATA::BOTH,  true);
  CALL_OP(equal, "==" 40, OPDATA::BOTH, true)
	CALL_OP(notEqual,	"!=",	40,		OPDATA::BOTH,	false	);	\
	CALL_OP(greater,	">",	50,		OPDATA::BOTH,	true	);	\
	CALL_OP(less,		"<",	50,		OPDATA::BOTH,	true	);	\
	CALL_OP(greaterEq,">=",	50,	OPDATA::BOTH,	true	);	\
	CALL_OP(lessEq, "<=",	50,	OPDATA::BOTH,	true	);	\
	CALL_OP(plus,"+", 61, OPDATA::BOTH, true	);	\
	CALL_OP(minus, "-",	61,		OPDATA::BOTH,		true	);	\
	CALL_OP(multiply,	"*",	71,		OPDATA::BOTH,		true	);	\
	CALL_OP(divide,	"/",	71,		OPDATA::BOTH,		true	);	\
	CALL_OP(mod,	"%",	70,		OPDATA::BOTH,		true	);	\
	CALL_OP(notOp,	"!",	74,		OPDATA::RIGHT,	true	);	\
	CALL_OP(increment,	"++",	75,		OPDATA::LEFT,		false	);	\
	CALL_OP(decrement,	"--",	75,		OPDATA::LEFT,		false	);	\
	CALL_OP(dot,		".",	81,		OPDATA::BOTH,	false	);	\
	CALL_OP(rightArrow,	"->",	83,		OPDATA::BOTH,	false	);	\
	CALL_OP(import,		"import",	90,		OPDATA::RIGHT,false	);	\
	CALL_OP(openParen,	"(",	100,	OPDATA::RIGHT,false	);	\
	CALL_OP(closeParen,	")",	99,		OPDATA::LEFT,	false	);	\
	CALL_OP(openSqBrac,	"[",	100,	OPDATA::BOTH,	false	);	\
	CALL_OP(closeSqBrac,"]",	99,		OPDATA::LEFT,	false	);	\
	CALL_OP(openCurl,	"{",	100,	OPDATA::RIGHT, false	);	\
	CALL_OP(closeCurl,"}",	99,		OPDATA::LEFT,	false	);	\

  OP_LIST;
 unordered_map<string, Operator>& getOPMap() {return opMap;}

	bool isOpenBrac(Operator op);
	bool isCloseBrac(Operator op);

private:

	OP_list();

	// used internally by the constructor
	void putOpInMap(OP op);

	// a hash map of all the operators
	unordered_map<string, Operator> opsMap;
};

// the single instance of this class, starts out as nullptr but chages to a real instance in AllOperators::init
//extern shared_ptr<AllOperators> ops;
extern OP_list* ops;
