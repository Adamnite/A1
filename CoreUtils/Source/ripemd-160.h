#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

unsigned int shl(unsigned int a, unsigned int s);
unsigned int f(int j, unsigned int x, unsigned int y, unsigned int z);
unsigned int K(int j);
unsigned int K_(int j);
unsigned int r(int j);
unsigned int r_(int j);
unsigned int s(int j);
unsigned int s_(int j);

void block_hash(unsigned int *X, unsigned int &h0, unsigned int &h1, unsigned int &h2, unsigned int &h3, unsigned int &h4);

string ripemd160_file(char *str);
string ripemd160_str(string str);

unsigned int inv(unsigned int a);