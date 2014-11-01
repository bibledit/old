// libbibledit.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libbibledit.h"
#include <iostream>
#include <stdexcept>
#include <library/bibledit.h>


using namespace std;


void bibledit_root_win(const char *directory)
{
  bibledit_root(directory);
}


void bibledit_start_win()
{
  bibledit_start();
}


bool bibledit_running_win()
{
  return bibledit_running();
}


void bibledit_stop_win()
{
  bibledit_stop();
}


