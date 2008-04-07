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


#ifndef INCLUDED_BIBLETIME_H
#define INCLUDED_BIBLETIME_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "gwrappers.h"


class BibleTime
{
public:
  BibleTime (bool dummy);
  ~BibleTime ();
  bool connected ();
  void sendreference (const Reference& reference);
  bool getreference (Reference& reference);
  vector<ustring> getbibles ();
  vector<ustring> getcommentaries ();
  vector<ustring> search_in_default_bible (const ustring& searchtext);
  vector<ustring> search_in_open_modules (const ustring& searchtext);
  vector<ustring> search_in_module (const ustring& modulename, const ustring& searchtext);
  void reloadmodules ();
private:
  vector<ustring> search (const ustring& modulename, const ustring& searchtext, int selector);
  ustring database ();
  void initialize_database ();
  void getmodules ();
};


#endif
