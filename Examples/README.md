# A1 Default Types and Syntax Considerations

A1, being a derivation of Python, should implement some form of optional type declration both for assignment and usage within functions. For example, designing a function for
adding two variables can be done by either declaring the variables and their respective values beforehand, or dynamically inputing them during the function call.
For the second option in A1, the dynamism of Python should be maintained (described below), along with optional type guards.

def add(x,y):<br>
  let c = x + y <br>
  return c
  

And with type guards:
def add(x:num, y:num): <br>
  let c:num = x + y <br>
  return c

Note that for the variable c, which was declared while being assigned, the type guard is also optional. For variables that are assigned, the "let" keyword should be present. 


In general, the following syntax should be used for the declration of various types: str (string), num (integers, floats, and doubles), and bool (booleans).
Map, array, etc should be used for more complex data types which need to be declared due to their functionality (in general, we should follow the Python paradigm).

For default types (regardless of whether a type guard is present) for numbers, the following should be used.
num: 64 bit (if int, it should be a uint64. For floats and doubles, an alternative methodlogy can be used).
bool: False and None should be equal; return 1 and return 0 should correspond to return True and return False respectively.

In addition, the following types should be supported. These types are optional, and must be declrared as a type guard. 

uint32, int32, uint256, int128, int256, uint128

Such a variable can be declared using the following terminlogy:
let x:uint32 = 22

In general, the syntax should blend both the simplicity of Python and the security of a modern smart-contract programming language.
