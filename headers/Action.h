#pragma once

#include "Frame.h"
#include "Type.h"
#include "CPP.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include "utils/stringDrawing.h"


#include <stdlib.h> // malloc() and free() on some systems

using std::shared_ptr;
using std::unique_ptr;
using std::to_string;
using std::function;
using std::vector;

class Action;

extern shared_ptr<Action> voidAction;

class Action
{
public:

	Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn);

	virtual ~Action() {}

	void setData(string in) {data = in;}

	//void setText(string in) {text=in;}
	//string getText() {return text;}
	string toString();
	string getTypesString();
	virtual bool isFunction() {return false;}

	Type& getReturnType() {return returnType;}
	Type& getInLeftType() {return inLeftType;}
	Type& getInRightType() {return inRightType;}
	//void* execute(void* inLeft, void* inRight);

	virtual string getData() {return data;}
	virtual void* execute(void* inLeft, void* inRight)=0;
	//virtual string getCSource(string inLeft="", string inRight="")=0;
	virtual void addtoProg(shared_ptr<Action> inLeft, shared_ptr<ActionData> inRight, CppProgram* prog)
	{
		prog->comment("Conversion '"+getDescription()+"' C++ Code not available for this type.");
	}
	void addtoProg(CPP* prog) {addToProg(voidAction, voidAction, prog);}
	// void addToProg(Action inLeft, Action inRight, CppProgram* prog)

	string nameH="";

protected:

	//string text;

	Type returnType;
	Type inLeftType;
	Type inRightType;
	string data;
	//virtual DataElem * privateExecute(DataElem * inLeft, DataElem * inRight)=0;
};

typedef shared_ptr<ActionData> Action;

class AstNodeBase;

//Action lambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn);
Action lambdaAction(Type inLeftTypeIn, Type inRightTypeIn, Type returnTypeIn, function<void*(void*,void*)> lambdaIn, function<void(Action inLeft, Action inRight, CppProgram* prog)> addCppToProg, string textIn);
Action createNewVoidAction();

Action branchAction(Action leftInputIn, Action actionIn, Action rightInputIn);

Action functionAction(Action actionIn, shared_ptr<StackFrame> stackFrameIn);
Action functionAction(unique_ptr<AstNodeBase> nodeIn, Type returnTypeIn, shared_ptr<StackFrame> stackFameIn);

Action andAction(Action firstActionIn, Action secondActionIn);
Action orAction(Action firstActionIn, Action secondActionIn);

Action ifAction(Action conditionIn, Action ifActionIn);
Action ifElseAction(Action conditionIn, Action ifActionIn, Action elseAction);

Action listAction(const std::vector<Action>& actionsIn, const std::vector<Action>& destroyersIn);

Action loopAction(Action conditionIn, Action loopActionIn);
Action loopAction(Action conditionIn, Action endActionIn, Action loopActionIn);

Action makeTupleAction(const std::vector<Action>& sourceActionsIn);
//Action getElemFromTupleAction(Action source, string name);
Action getElemFromTupleAction(Type source, string name);
Action cppTupleCastAction(Action actionIn, Type returnType);

Action varGetAction(size_t in, Type typeIn, string idIn);
Action varSetAction(size_t in, Type typeIn, string idIn);
Action globalGetAction(size_t in, Type typeIn, string idIn);
Action globalSetAction(size_t in, Type typeIn, string idIn);

class NamespaceData;
Action constGetAction(const void* in, Type typeIn, string idIn, shared_ptr<NamespaceData> ns);

Action typeGetAction(Type typeIn);
