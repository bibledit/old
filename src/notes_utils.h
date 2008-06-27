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

#ifndef INCLUDED_NOTES_UTILS_H
#define INCLUDED_NOTES_UTILS_H

#include "libraries.h"
#include <gtk/gtk.h>
#include "reference.h"

#define TABLE_NOTES "notes"

void notes_database_verify(bool gui);
ustring notes_database_filename();
gint notes_database_get_unique_id();
void notes_delete_one(int id);
void notes_sort(vector<unsigned int>& ids, const vector<ustring>& refs, const vector<ustring>& allrefs, const vector<int>& dates);
void notes_select(vector<unsigned int>& ids, unsigned int& id_cursor, const ustring& currentreference);
void notes_display(ustring& note_buffer, vector <unsigned int> ids, unsigned int cursor_id, unsigned int& cursor_offset, bool& stop);
void notes_get_references_from_editor(GtkTextBuffer *textbuffer, vector<Reference>& references, vector<ustring>& messages);
ustring notes_categories_filename();
void notes_categories_check();
void notes_categories_add_from_database(vector<ustring>& categories);
void notes_projects_add_from_database(vector<ustring>& projects);
void notes_vacuum();
void notes_store_one(int id, ustring& note, const ustring& project, vector<Reference>& references, const ustring& category, int date_created, const ustring& user_created, ustring& logbook);
void notes_change_category(const ustring& from, const ustring& to);
void notes_change_project(const ustring& from, const ustring& to);
void notes_read(vector <unsigned int> ids, vector <ustring>& data);
void notes_update_old_one(ustring& note);
gchar * notes_cursor_anchor ();

#endif
