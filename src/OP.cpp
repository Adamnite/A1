#include "../headers/OP_List.h"
#include "../headers/ErrorHandler.h"

OP_List* ops=nullptr;

OP Create(string Input, int leftPrecedence, int rightPrecedence, bool isoverload);

void OP_List::init()
{
  OPs = new OP_List();


}

OP_List::OP_List()
{
  #undef DECLARE_OP

  #define DECLARE_OP(operation, op_text, precedence, input, isoverload);

  ALL_OPS;
}

void OP_List::putOpInMap(OP op)
{
  opsMap[op->getText()]= op;
}
void OP_List::get(string op_text, vector<OP>& out)
{
  int left = 0
  int end = op_text.size()

  while (left < int(op_text.size()))
  {
    while (true)
    {
      if (end <= left)
      {
         error.log("Operator not defined" + SOURCE_ERROR);

      }
      auto i = opsMap.find(op_text.substr(left, end-left));

      if (i == opsMap.end())
      {
        end--;
      }
      else
      {
        out.push_back(i->second);
        left = end;
        end = op_text.size();
        break;
      }
    }
  }
}

bool ALL_OPs::isOpenBrac(OP op)
{
  return op == OpenParen || op == openSqBrack || op == openCrBrac;

}

bool AllOps::isCloseBrac(OP op)
{
  return op==closePeren || op==closeSqBrac || op==closeCrBrac;
}


}
