/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef INCLUDED_UNITTESTS_LIBRARIES_H
#define INCLUDED_UNITTESTS_LIBRARIES_H


#include <config/libraries.h>


void test_sqlite ();
void test_session_logic ();
void test_empty_folders ();
void test_flate ();
void test_checksum_logic ();
void test_store_bible_data ();
void test_editor_html2usfm ();
void test_editor_usfm2html ();
void test_editor_roundtrip ();
void test_editor_development ();
void test_workbench_logic ();
void test_client_logic ();
void test_check_sentences ();
void test_check_versification ();
void test_check_usfm ();
void test_check_verses ();
void test_check_pairs ();
void test_hyphenate ();
void test_search_logic ();
void test_libraries_temporal ();
void test_json ();
void test_related ();


#endif
