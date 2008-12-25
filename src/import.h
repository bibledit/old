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


#ifndef INCLUDED_IMPORT_H
#define INCLUDED_IMPORT_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"


class ImportBookRead
{
public:
  ImportBookRead (const ustring& filename, const ustring& encoding);
  void usfm ();
  void bibleworks ();
  void mechonmamre ();
  void onlinebible ();
  vector<ustring> lines;
  ustring bookname;
private:
  vector<ustring> rawlines;
};


gchar * unicode_convert (gchar *data, const ustring& encoding);


void import_dialog_selector (bool& structured, bool& raw);
ustring import_type_human_readable (ImportType importtype);
ustring bibleworks_file_get_bookname (const ustring& filename);
vector<ustring> bibleworks_file_divide (const ustring& inputfile);
bool mechon_mamre_copyright (const ustring& inputfile);
vector<ustring> mechon_mamre_produce_master_files (const vector<ustring>& inputfiles);
bool online_bible_file (const ustring& filename);
bool online_bible_parse_reference (ustring line, unsigned int& book, unsigned int& chapter, unsigned int& verse);
vector <ustring> online_bible_file_divide (const ustring& inputfile);


#endif
