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


#ifndef INCLUDED_KEYTERMS_H
#define INCLUDED_KEYTERMS_H


#include "libraries.h"
#include "reference.h"
#include "progresswindow.h"


ustring keyterms_reference_start_markup ();
ustring keyterms_reference_end_markup ();
void keyterms_import_textfile(const ustring& textfile, ustring category);
void keyterms_import_otkey_db(const ustring& textfile, ustring category);
void keyterms_import_ktref_db(const ustring& textfile, ustring category);
void keyterms_import_ktbh_txt(const ustring& textfile, ustring category);
vector <ustring> keyterms_get_categories();
void keyterms_delete_collection (const ustring& collection);
void keyterms_get_terms(const ustring& searchterm, const ustring& collection, vector <ustring>& terms, vector <unsigned int>& ids);
bool keyterms_get_term(unsigned int id, ustring& term);
bool keyterms_get_data(unsigned int id, ustring& category, ustring& comments, vector<Reference>& references);
void keyterms_retrieve_renderings(const ustring& project, const ustring& keyterm, const ustring& collection, vector<ustring>& renderings, vector<bool>& wholewords, vector<bool>& casesensitives);
void keyterms_store_renderings(const ustring& project, const ustring& keyterm, const ustring& collection, const vector<ustring>& renderings,  const vector<bool>& wholewords, const vector<bool>& casesensitives);
void keyterms_export(const ustring& directory, bool gui);
vector <int> keyterms_get_terms_in_verse(const Reference& reference);
deque <ustring> keyterms_rendering_retrieve_terms(const ustring& project, const ustring& rendering);
int keyterms_retrieve_highest_id (const gchar * table);
void keyterms_export_renderings (const ustring& collection, bool include_terms_wo_renderings);


#endif
