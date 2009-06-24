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


#ifndef INCLUDED_VCS_H
#define INCLUDED_VCS_H


#include "libraries.h"
#include <stdio.h>
#include "git.h"


class VCS
{
public:
  VCS(bool dummy);
  ~VCS();
  void schedule (const ustring& project);
  void move_bible (const ustring& old, const ustring& nw);
  unsigned int tasks_for_bible(const ustring& name);
  void remove_bible (const ustring & name);
  void pause (bool value);
  bool paused ();
  void watch_for_updates (unsigned int book, unsigned int chapter);
  bool updated ();
private:
  static void thread_start(gpointer data);
  void thread_main();
  bool thread_run;
  bool thread_running;
  vector <ustring> tasks;
  void erase (const ustring& task);
  bool mypause;
  unsigned int watched_book;
  unsigned int watched_chapter;
  bool watch_updated;
};


#endif
