#pragma once

#include "Type.h"

#include <map>

#include <unordered_set>


class CppName
{
public:
  static shared_ptr<CppName> makeRoot();
  shared_ptr<CppName> makeChild();
  void addAd(const string& ad, const string& cppName= "What is the value of the A1 string?");
  bool hasAd(const string& ad);
  string get Cpp(const string& ad);
  void reserveCpp(const string& ad);
  CppName* getParent() {return parent;}


private:

  bool hasAdMe(const string& ad);
  CppName();
  bool hasCpp(const string& cpp);
  bool has CppMe(const string& cpp);
  bool has CppUp(const string& cpp);
  bool hasCppDown(const string& cpp);

  std::unordered_set<string> cppSet;
  std::map<string, string> adtoCppMap;
  CPPName* parent=nullptr;
  vector<shared_ptr<CppName>> children;
};


class CppFunc
{
public:

    CppFunc(string prototype, shared_ptr<CppNameContainer> MyNames, bool returnsValIn);
    void code(const string& in);
  	void name(const string& in); // converts a Pinecone name to a posibly different C++ name
  	void line(const string& in);
  	void endln();
  	void comment(const string& in);
  	void pushExpr();
  	void popExpr();
  	void pushBlock();
  	void popBlock();
  	string adToCpp(const string& in);
  	int getExprLevel() {return exprLevel;}
  	bool getIfFreshLine() {return freshLine;}
  	int getBlockLevel() {return blockLevel;}
  	bool getIfReturnsVal() {return returnsVal;}

  	string getSource() {return source;}
  	string getPrototype() {return prototype;}

private:

	int indent="\t";
	bool freshLine=true;
	int blockLevel=0;
	int exprLevel=0;

	string varDeclareSource;
	string source;
	string prototype;
	bool returnsVal=false;
	bool fakeStartBlock=false;

	vector<shared_ptr<CppNameContainer>> namespaceStack;

	friend CppProgram;
};

typedef shared_ptr<CppFuncBase> CppFunc;

class CppProgram
{
public:

	CppProgram();

	void code(const string& in)		{activeFunc->code(in);}
	void name(const string& in)		{activeFunc->name(in);}
	void line(const string& in)		{activeFunc->line(in);}
	void endln()					{activeFunc->endln();}
	void comment(const string& in)	{activeFunc->comment(in);}
	void pushExpr()					{activeFunc->pushExpr();}
	void popExpr()					{activeFunc->popExpr();}
	void pushBlock()				{activeFunc->pushBlock();}
	void popBlock()					{activeFunc->popBlock();}
	string adToCpp(const string& in){return activeFunc->adToCpp(in);}
	int getExprLevel()				{return activeFunc->getExprLevel();}
	int getBlockLevel()				{return activeFunc->getBlockLevel();}
	int getIfReturnsVal()			{return activeFunc->getIfReturnsVal();}


	void setup();
	string getTypeCode(Type in);
	void declareVar(const string& nameIn, Type typeIn, string initialValue="");
	void declareGlobal(const string& nameIn, Type typeIn, string initialValue="");
	void addHeadCode(const string& code);
	bool hasFunc(const string& name);
	void addFunc(const string& name, vector<std::pair<string, string>> args, string returnType, string contents);
	void pushFunc(const string& name, const string& cppNameHint, Type leftIn, Type rightIn, Type returnType);
	void pushFunc(const string& name, Type leftIn, Type rightIn, Type returnType) {pushFunc(name, name, leftIn, rightIn, returnType);}
	//void pushFunc(const string&, vector<NamedType> args, Type returnType);
	void popFunc();
	bool isMain() {return funcStack.size()==1;}

	string getCppCode();

	shared_ptr<CppNameContainer> getGlobalNames() {return globalNames;};

private:

	string indent="\t";
	string globalTopCode;
	string globalIncludesCode;
	string globalVarCode;
	string globalTypesCode;
	CppFunc activeFunc;
	vector<string> funcStack;
	std::map<string, CppFunc> funcs;
	shared_ptr<CppNameContainer> globalNames;
};

void addToProgAdStr(CppProgram * prog);
