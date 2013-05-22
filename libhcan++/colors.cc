
#ifdef __WIN32__

#include <stdio.h>
#include <stdarg.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
#include <wincon.h>

#include <iostream>

using namespace std;

ostream &COLOR_NORMAL (ostream &s)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED | 
			FOREGROUND_BLUE | FOREGROUND_GREEN);
	return s;
}

ostream &COLOR_GREEN (ostream &s)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	return s;
}

ostream &COLOR_BLUE (ostream &s)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	return s;
}

ostream &COLOR_RED (ostream &s)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_INTENSITY | FOREGROUND_RED);
	return s;
}

#endif

