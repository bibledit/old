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
#include <set>


using namespace std;


GtkWidget *window;
GtkWidget *label;
int action_count;


gchar * maintenance_db_filename = NULL;


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
void trim_snapshots (string filename);
unsigned int convert_to_int(const string& str);
void trim_snapshots_by_group (sqlite3 *db, const vector <unsigned int>& group, unsigned int book, unsigned int chapter, int spacing);
static bool on_timeout (gpointer data);
bool handle_shell_command (sqlite3 *db);
string double_apostrophy(const string & line);
void feedback ();
bool handle_git_repositories (sqlite3 *db);
bool handle_snapshots (sqlite3 *db);
bool handle_databases (sqlite3 *db);


#endif
