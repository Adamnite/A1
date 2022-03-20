#pragma once

class Lexical;

#include <memory>
using std::shared_ptr;


#include <string>
usign std::string;

class Op_All;

class Op_Data
{
public:

  enum Input
  {
    LEFTIN,
    RIGHTIN,
    BOTH,
  };


  string getInput() {return text;}
  int getPrecendence() {return precedence}
  bool OverFlow() {return isoverload;}
  bool isLeftIn() {return input == BOTH || input == LEFTIN;}
  bool isRightIn() {return input == BOTH || input == RIGHTIN;}

private:
  friend OP_list;
  OpData(string op_input, int precedenceIn, Input inputIn, bool overloadIn)
  {
    op_text = op_input;
    precedence = precedence_input;
    input  = inputIn;
    overloadable = overloadableIn;
  }

  string op_text;

  int precedence;

  bool isoverload;

  InputTake input;

};

typedef shared_ptr<OP_DATA> Op;
