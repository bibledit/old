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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#ifndef INCLUDED_KEYTERMS_H
#define INCLUDED_KEYTERMS_H


#include "libraries.h"
#include "reference.h"
#include "progresswindow.h"


vector<ustring> keyterms_get_raw_files ();
void keyterms_create_database ();
void keyterms_import_textfile (const ustring& textfile);
void keyterms_import_otkey_db ();
void keyterms_import_ktref_db ();
void keyterms_import_ktbh_txt ();  
vector <ustring> keyterms_get_categories ();
void keyterms_get_terms (const ustring& searchterm, 
                         const ustring& collection,
                         vector<ustring>& terms, 
                         vector<unsigned int>& levels, 
                         vector<unsigned int>& parents, 
                         vector<unsigned int>& ids);
bool keyterms_get_term (unsigned int id, ustring& term, unsigned int& parent);
bool keyterms_get_data (unsigned int id, ustring& category, ustring& comments,
                        vector<Reference>& references, vector<ustring>& related);
void keyterms_retrieve_renderings (const ustring& project,
                                   const ustring& keyterm, 
                                   const ustring& collection,
                                   vector<ustring>& renderings,
                                   vector<bool>& wholewords,
                                   vector<bool>& casesensitives);
void keyterms_store_renderings (const ustring& project, 
                                const ustring& keyterm, 
                                const ustring& collection, 
                                const vector<ustring>& renderings,
                                const vector<bool>& wholewords,
                                const vector<bool>& casesensitives);
void keyterms_export (const ustring& directory, bool gui);


#endif
