#pragma once


#include "Type.h"
#include "Action.h"
#include "Operator.h"
#include "AST.h"

#include <unordered_map>
using std::unordered_map;

#include <vector>
using std::vector;

class Stack;
class NamespaceData;
typedef shared_ptr<NamespaceData> Namespace;

//Defines the analysis of encapsulated namespaces, such as
// multiple conditionals within a single function. Analysis is
// done by splitting the broader namespace into a hash table.

class NamespaceData: public std::enable_shared_from_thos<NamespaceData>
{
public:
      static Namespace makeRoot();

      Namespace makeChild();
      Namespace makeChildandFrame(string namein);
      string getString();
      string getStringWithParents();
      shared_ptr<Frame> getFrame() {return Frame;}
      void setInput(Type left, Type right);
      void addNode(ASTNode node, string id);
      Type getType(string name, boo throwSourceError, Token tokenForError);
      Action getCopier(Type type);
      Action getactionfortokenwithinput(Token token, Type left, Type right, bool dynamic, bool throwSourceError, Token tokenForError);
      vector<Action>* getDestroyer() {return &destructorActions;}
      Action wrapInDestroyer(Action in);

 private:

    void getnodes(vector<AstNodeBase*>& out, string text, bool checkActions, bool checkDynamic);

    void nodesToMatchingActions(vector<Action>& out, vector<AstNodeBase*>& nodes, Type leftInType, Type rightInType);

    class IdMap
    {
    public:
        void add(string key, AstNode node);
        void get(string key, vector<AstNodeBase* & out);


    private:
        unordered_map<string, vector<AstNode>> nodes;
      };


      Action addVar(Type type, string name);

      NamespaceData(Namespace parentIn, shared_ptr<Frame> stackFrameIn, string nameIn= "");

      string myName;

      shared_ptr<NamespaceData> parent;

      shared_ptr<StackFrame> stackFrame;
      IdMap actions;
      IdMap dynamicActions;


      IdMap types;


      IdMap copiers;

      vector<Action> destructorActions;
    };
