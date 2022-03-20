#pragma once

#include <vector>
using std::vector;


#include "Type.h"

extern void* globalFramePtr;
extern void* stackPtr;


class Frame
{
public:
  void addMember (Type in);

  void Set(Type left, Type right);
  
}
