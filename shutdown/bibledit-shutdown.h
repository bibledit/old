/*
** Copyright (©) 2003-2008 Teus Benschop.
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


#ifndef INCLUDED_BIBLEDIT_HELP_H
#define INCLUDED_BIBLEDIT_HELP_H


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


using namespace std;


GtkWidget *splashscreen;
GtkWidget *progressbar;


class SqliteReader
{
public:
  SqliteReader (int dummy);
  ~SqliteReader ();
  static int callback (void *userdata, int argc, char **argv, char **column_names);
  vector<string> string0;
  vector<string> string1;
  vector<string> string2;
  vector<string> string3;
private:
};




int main (int argc, char *argv[]);


#endif
