#include "chip8const.h"
#include <random>
#include<ctime>
#include <string>
#include<sstream>
#include <iomanip>
using namespace std;

string decode(unsigned short _code);
string decodeGeneral(unsigned short _code);
string decode8000(unsigned short _code);
string decodeE000(unsigned short _code);
string decodeF000(unsigned short _code);
