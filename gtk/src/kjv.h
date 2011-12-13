/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_KJV_H
#define INCLUDED_KJV_H


#include "libraries.h"
#include "reference.h"
#include <glib.h>
#include <sqlite3.h>
#include "htmlwriter2.h"


void kjv_home_entry (HtmlWriter2& htmlwriter);
ustring kjv_html_entry_url ();
void kjv_detailed_page (HtmlWriter2& htmlwriter);
ustring kjv_create_database_url ();
void kjv_action_page (HtmlWriter2& htmlwriter);
ustring kjv_sql_filename ();
void kjv_import_sword (const ustring& textfile, const ustring& database);



void kjv_import_sword ();
void kjv_import_zefania ();
void kjv_get_strongs_data (const Reference& reference, vector <ustring>& strongs, vector <ustring>& words);
vector <Reference> kjv_get_strongs_verses (const ustring& strongs);


#endif
