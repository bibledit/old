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


#ifndef INCLUDED_EXPORT_UTILS_H
#define INCLUDED_EXPORT_UTILS_H


#include "libraries.h"
#include <gtk/gtk.h>


enum ExportType {etBible, etReferences, etStylesheet, etNotes, etKeyterms};
enum ExportBibleType {ebtUSFM, ebtBibleWorks, ebtOSIS, ebtSWORD, ebtOpenDocument, ebtGoBible};
enum ExportOsisType {eotRecommended, eotGoBibleCreator, eotOld};
enum ExportUsfmType {eutEverything, eutChangesOnly};
enum ExportUsfmChangesType {euctSinceLast, euctSinceDateTime};


void export_to_usfm (const ustring& project, ustring location, bool zip);
void export_to_usfm_changes (const ustring& project, int time_from, ustring comment);
void export_to_bibleworks (const ustring& project, const ustring& filename);
void export_translation_notes (const ustring& filename, const vector<unsigned int> & ids_to_display, bool export_all);
void export_to_osis_recommended (const ustring& project, const ustring& filename);
void export_to_osis_old (const ustring& project, const ustring& filename);
void export_to_osis_for_go_bible_creator (const ustring& project, const ustring& filename);
void export_to_sword (const ustring& project, ustring directory);
void export_to_opendocument (const ustring& project, const ustring& filename);
void export_to_go_bible (const ustring& project, const ustring& foldername);


#endif
