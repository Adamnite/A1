#pragma once
#include "stringUtils.h"

namespace str
{

  string chartostring(unsigned char c);

  string inttoBase62(unsifned int in, int maxDigits = -1);

  string ptrtoUniqueStr(void* ptrIn, int digits = 4);
}
