/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#ifndef INCLUDED_TINY_UTILITIES_H
#define INCLUDED_TINY_UTILITIES_H


#include <string>
#include <vector>
#include <glib.h>


using namespace std;


void tiny_spawn_write (int fd, const string& text);


class Spawn
{
public:
  Spawn (const string& program);
  ~Spawn ();
  void workingdirectory (string directory);
  void arg (string value);
  void run ();
  bool result;
  gint exitstatus;
  gchar *standard_output;
  gchar *standard_error;
private:
  string myprogram;
  string myworkingdirectory;
  vector <string> arguments;
};


#endif
