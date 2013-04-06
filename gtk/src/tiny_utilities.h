/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


#include "libraries.h"


unsigned int convert_to_int(const ustring & str);
ustring convert_to_string(int i);
ustring convert_to_string(unsigned int i);
ustring convert_to_string(bool b);
ustring convert_to_string(double d);
ustring convert_to_string(long unsigned int i);
bool convert_to_bool(const ustring & s);
double convert_to_double(const ustring & s);
ustring convert_bool_to_yes_no(bool b);
ustring trim(const ustring & s);
string trim(const string & s);
ustring tiny_gw_build_filename(const ustring& part1, const ustring& part2);
ustring tiny_gw_build_filename(const ustring& part1, const ustring& part2, const ustring& part3);
ustring tiny_directories_get_root();
ustring tiny_directories_get_projects();
ustring tiny_project_directory(const ustring& project);
ustring tiny_project_data_directory_part();
ustring tiny_project_data_directory_project(const ustring& project);
void tiny_spawn_write (int fd, const ustring& text);


class TinySpawn
{
public:
  TinySpawn (const char * program);
  ~TinySpawn ();
  void workingdirectory (ustring directory);
  void arg (ustring value);
  void read ();
  void run ();
  bool result;
  gint exitstatus;
  vector <ustring> standardout;
  vector <ustring> standarderr;
private:
  const char * myprogram;
  ustring myworkingdirectory;
  bool myread;
  vector <ustring> arguments;
};


class ParseLine
{
public:
  ParseLine (const ustring & text);
  ~ParseLine ();
  vector <ustring> lines;
private:
};


#endif
