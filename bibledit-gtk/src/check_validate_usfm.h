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


#ifndef INCLUDED_CHECK_VALIDATE_USFM_H
#define INCLUDED_CHECK_VALIDATE_USFM_H


#include "libraries.h"
#include "progresswindow.h"


class CheckValidateUsfm
{
public:
  CheckValidateUsfm (const ustring& project, 
                     const vector<unsigned int>& books, 
                     bool gui,
                     bool checksheet);
  ~CheckValidateUsfm ();
  vector<ustring> references;
  vector<ustring> comments;
  bool cancelled;
private:
  unsigned int book;
  unsigned int chapter;
  ustring verse;
  bool mychecksheet;
  set<ustring> styles;
  void check (const ustring& text);
  void deprecated_marker (const ustring& marker);
  ustring usfm_extract_marker_with_forwardslash (ustring & line);
  void check_on_endmarker (ustring& line, const ustring& marker, bool optional);
  void message (const ustring& message);
  ProgressWindow * progresswindow;
};


#endif
