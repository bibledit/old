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
  cout << "Set bibledit root to " << directory << endl;
  bibledit_root(directory);
}


void bibledit_start_win()
{
	cout << "Call bibledit_start" << endl;
  bibledit_start();
}


bool bibledit_running_win()
{
  cout << "Call bibledit_running" << endl;
  return bibledit_running();
}


void bibledit_stop_win()
{
  cout << "Call bibledit_stop" << endl;
  bibledit_stop();
}


