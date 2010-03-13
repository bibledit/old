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


#ifndef INCLUDED_MECHON_MAMRE_H
#define INCLUDED_MECHON_MAMRE_H


#include "libraries.h"
#include "htmlwriter2.h"
#include "reference.h"


ustring mechon_mamre_html_entry_url ();
ustring mechon_mamre_download_url ();
ustring mechon_mamre_import_url ();
void mechon_mamre_home_entry (HtmlWriter2& htmlwriter);
void mechon_mamre_detailed_page (HtmlWriter2& htmlwriter);
void mechon_mamre_action_page (HtmlWriter2& htmlwriter);
bool mechon_mamre_copyright (const ustring& inputfile);
void mechon_mamre_extract_book_chapter (const ustring& file, unsigned int& book, unsigned int& chapter);
vector <ustring> mechon_mamre_extract_contents (const ustring& file, unsigned int chapter);


#endif
