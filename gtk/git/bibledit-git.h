/*
** Copyright (©) 2003-2008 Teus Benschop.
** Copyright (©) 2011 David Gardner.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_BIBLEDIT_GIT_H
#define INCLUDED_BIBLEDIT_GIT_H


#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <set>
#include <deque>


using namespace std;


GtkTextBuffer * buffer = NULL;
GtkWidget * textview = NULL;
GtkWidget * checkbutton = NULL;
deque <string> folders;
bool we_loop;


class TinySpawn
{
public:
  TinySpawn (const char * program);
  ~TinySpawn ();
  void workingdirectory (string directory);
  void arg (string value);
  void read ();
  void run ();
  bool result;
  gint exitstatus;
  vector <string> standardout;
  vector <string> standarderr;
private:
  const char * myprogram;
  string myworkingdirectory;
  bool myread;
  vector <string> arguments;
};


class ParseLine
{
public:
  ParseLine (const string & text);
  ~ParseLine ();
  vector <string> lines;
private:
};


int main (int argc, char *argv[]);
static bool on_timeout (gpointer data);
void message (string message);
string trim(const string & s);
string convert_to_string(int i);
string convert_to_string(unsigned int i);
string convert_to_string(bool b);
string convert_to_string(double d);
string convert_to_string(long unsigned int i);
static void on_checkbutton_loop_toggled (GtkToggleButton *togglebutton, gpointer user_data);
bool exit_timeout (gpointer data);


#endif
