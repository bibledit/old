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


#ifndef INCLUDED_GWRAPPERS_H
#define INCLUDED_GWRAPPERS_H


#include "libraries.h"
#include <glib.h>


ustring gw_build_filename (const ustring& part1, const ustring& part2);
ustring gw_build_filename (const ustring& part1, const ustring& part2, const ustring& part3);
ustring gw_build_filename (const ustring& part1, const ustring& part2, const ustring& part3, const ustring& part4);
ustring gw_build_filename (const ustring& part1, const ustring& part2, const ustring& part3, const ustring& part4, const ustring& part5);
void gw_message (const ustring& message);
void gw_warning (const ustring& warning);
void gw_critical (const ustring& critical);
void gw_error (const ustring& error);
ustring gw_path_get_basename (const ustring& filename);
ustring gw_path_get_dirname (const ustring& filename);
bool gw_find_program_in_path (const ustring& program);
void gw_destroy_source (guint& event_id);
void gw_mkdir_with_parents (const ustring& directory);


class GwSpawn
{
public:
  //  GwSpawn (const char * program);
  GwSpawn(const ustring &program);
  ~GwSpawn ();
  void workingdirectory (ustring directory);
  void arg (ustring value);
  void async ();
  void devnull ();
  void write (ustring text);
  void read ();
  void progress (ustring text, bool allow_cancel);
  void describe ();
  void run ();
  bool cancelled;
  bool result;
  gint exitstatus;
  GPid pid;
  vector <ustring> standardout;
  vector <ustring> standarderr;
private:
  //const char * myprogram;
  ustring myprogram;
  ustring myworkingdirectory;
  bool myasync;
  bool mydevnull;
  ustring mywrite;
  bool myread;
  bool myprogress;
  bool myallowcancel;
  bool myhide;
  ustring mytext;
  vector <ustring> myarguments;
};


#endif
