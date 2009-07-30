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


#ifndef INCLUDED_IMPORT_H
#define INCLUDED_IMPORT_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "types.h"


enum ImportType {itBible, itReferences, itStylesheet, itNotes, itKeyterms};
enum ImportBibleType {ibtUsfm, ibtBibleWorks, ibtMechonMamre, ibtOnlineBible, ibtRawText};



class ImportBookRead
{
public:
  ImportBookRead (const ustring& filename, const ustring& encoding);
  void mechonmamre ();
  void onlinebible (map <ustring, unsigned int> bookmap);
  vector<ustring> lines;
  ustring bookname;
private:
  vector<ustring> rawlines;
};


gchar * unicode_convert (gchar *data, const ustring& encoding);


ustring bibleworks_file_get_bookname (const ustring& filename);
bool mechon_mamre_copyright (const ustring& inputfile);
vector<ustring> mechon_mamre_produce_master_files (const vector<ustring>& inputfiles);
bool online_bible_file (const ustring& filename);
bool online_bible_parse_reference (ustring line, unsigned int& book, unsigned int& chapter, unsigned int& verse, map <ustring, unsigned int>& bookmap);
vector <ustring> online_bible_file_divide (const ustring& inputfile, map <ustring, unsigned int> bookmap);
void import_check_usfm_files (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages);
void import_usfm_file (const ustring& file, unsigned int book, const ustring& project, vector <ustring>& messages);
void import_check_bibleworks_file (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages);
void import_bibleworks_file (const ustring& file, const ustring& bible, vector <ustring>& messages);


#endif
