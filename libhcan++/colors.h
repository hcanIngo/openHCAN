#ifndef COLORS_H
#define COLORS_H

#ifndef __WIN32__

#define COLOR_NORMAL "\033[30;0m"
#define COLOR_RED "\033[31;2m"
#define COLOR_GREEN "\033[32;2m"
#define COLOR_3 "\033[33;2m"
#define COLOR_BLUE "\033[34;1m"
#define COLOR_5 "\033[35;2m"

#else

#include <iostream>

using namespace std;

ostream &COLOR_NORMAL (ostream &s);
ostream &COLOR_GREEN (ostream &s);
ostream &COLOR_BLUE (ostream &s);
ostream &COLOR_RED (ostream &s);

#endif

#endif

