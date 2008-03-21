/*
** Copyright (©) 2003-2006 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <gtk/gtk.h>


using namespace std;


/*
This helper program vacuums a database for Bibledit.

If Bibledit shuts down, it will call this helper program with the filename
of the database that needs to be vacuum-ed on the commandline.

The result will be faster shutdown of Bibledit.

*/

int main(int argc, char *argv[])
{
  string filename = argv[1];
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    rc = sqlite3_open(filename.c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 2000);
    rc = sqlite3_exec (db, "vacuum;", NULL, NULL, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error (error);
    }
  }
  catch (exception & ex)
  {
    string message (ex.what ());
    g_critical ("%s", message.c_str());
  }
  sqlite3_close (db);
  return 0;
}
