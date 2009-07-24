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


#ifndef INCLUDED_SCRIPTURECHECKS_H
#define INCLUDED_SCRIPTURECHECKS_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "checks.h"


bool scripture_checks_count_usfms (bool gui);
bool scripture_checks_validate_usfms (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_compare_usfms (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_chapters_verses (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_count_characters (bool gui);
bool scripture_checks_unwanted_patterns (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_capitalization (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_repetition (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_matching_pairs (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_unwanted_words (WindowReferences * references_window, CollectCheckingResults * results);
bool scripture_checks_word_inventory (bool gui);
bool scripture_checks_usfm_spacing (WindowReferences * references_window, CollectCheckingResults * results);
ustring checks_abbreviations_get_filename (const ustring& project);
ustring checks_uncapitalized_prefixes_get_filename (const ustring& project);
ustring checks_capitalized_suffixes_get_filename (const ustring& project);
ustring checks_repetition_show_only_get_filename (const ustring& project);
ustring checks_repetition_ignore_get_filename (const ustring& project);
ustring checks_unwanted_patterns_get_filename (const ustring& project);
ustring checks_unwanted_words_get_filename (const ustring& project);
bool scripture_checks_references_inventory (bool gui);
bool scripture_checks_validate_references (WindowReferences * references_window, CollectCheckingResults * results);
void scripture_checks_nt_quotations_from_ot (WindowReferences * references_window);
void scripture_checks_synoptic_parallels_from_nt (WindowReferences * references_window);
void scripture_checks_parallels_from_ot (WindowReferences * references_window);
bool scripture_checks_sentence_structure (WindowReferences * references_window, CollectCheckingResults * results);


#endif
