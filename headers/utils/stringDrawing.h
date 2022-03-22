#pragma once

#include "stirngUtils.h"

namespace string
{

  string getBoxedString(const string& in, string boxName = "", bool showLineNums = false, bool alwaysWidthMax = false, int maxWidth = -1);

  void putArrayinTreeNodeBox(vector<string>& data);

  string putStringInTreeNodeBox(vector<string>& data);

  string putStringInTreeNodeBox(const string& in);

  string makeList(vector<string>& data);
  string makeRootUpBinaryTree(const string& root, const string& leftBranch, const string& rightBranch, const string& leftLeaf, const string& rightLeaf)
}
