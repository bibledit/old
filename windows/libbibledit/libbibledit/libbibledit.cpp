// libbibledit.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libbibledit.h"
#include <iostream>
#include <stdexcept>
#include <library/bibledit.h>


using namespace std;


void bibledit_start_win()
{
	bibledit_start();
	cout << "Hello from libbibledit Windows" << endl;
}


