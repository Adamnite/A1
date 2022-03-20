#pragma once

#include <string>
using std::string;



#include <vector>
using std::vector;


#include <memory>
using std::shared_ptr;
using std::unique_ptr;


#include <cstring>
using std::memcpy;


class Types;

typedef shared_ptr<Types> Type;

const extern Type Undefined;
const extern Type Bool;
const extern Type Byte;
const extern Type Int;
const extern Type UInt; //Unsigned Int
const extern Type Decimal;
const extern Type Void;
extern Type String;
// Add specific blockchain types (address, more bits)



class CPPprogram;
class Action;


struct DefinedType
{
  string Define;
  Type type;
};

struct OffsetAndType
{
  size_t offset;
  Type type;
};

class Types: public std::enable_shared_from_this<Types>
{
public:

    virtual ~Types() = default;

    enum BasicType
    {
      UNDEFINED,
      BOOL,
      BYTE,
      INT,
      UINT //Unsigned Int
      DECIMAL,
      VOID,
      STRING,
      METATYPE,

    };

    static Type makeNewVoid();
    static Type makeNewPrimitive(PrimitiveType typeIn);
    Type getMeta();
    type getPtr();

    static string getString(PrimitiveType in);
    virtual string getString()=0;
    string getName() {return nameH}
    virtual string getCompactString()=0;
  	virtual string getCppLiteral(void * data, CppProgram * prog)=0;

  	virtual bool isCreatable() {return true;};
  	virtual bool isVoid() {return false;};
  	virtual bool isWhatev() {return false;};

  	virtual size_t getSize()=0;

  	virtual PrimitiveType getType()=0;

  	bool matches(Type other);

  	virtual Type getSubType() {return Void;}
  	virtual OffsetAndType getSubType(string name) {return {0, nullptr};}

  	virtual vector<NamedType>* getAllSubTypes(); // will thow an error if this is not a tuple type

  protected:

  	virtual bool matchesSameTypeType(Type other)=0;

  	Type ptrToMe=nullptr;
  };

  Type makeTuple(const vector<NamedType>& in, bool isAnonymous);

  //	since types are immutable, this class is an easy way to construct a tuple type
  class TupleTypeMaker
  {
  public:
  	TupleTypeMaker();
  	void add(string name, Type type);

  	void add(Type type);

  	Type get(bool isAnonymous);

  private:

  	string getUniqueName();

  	std::unique_ptr<vector<NamedType>> subTypes;
  };
