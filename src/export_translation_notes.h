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


#ifndef INCLUDED_EXPORT_TRANSLATION_NOTES_H
#define INCLUDED_EXPORT_TRANSLATION_NOTES_H


#include "libraries.h"
#include <gtk/gtk.h>
#include "export_utils.h"
#include <sqlite3.h>
#include "progresswindow.h"


class ExportTranslationNotes
{
public:
  ExportTranslationNotes (const ustring& filename, const vector<unsigned int> & ids_to_display, bool export_all);
  ~ExportTranslationNotes ();
private:
  sqlite3 *db;
  char *error;
  int rc;
  int notes_count;
  int note_counter;
  bool my_export_all;
  set<gint> * my_ids;
  WriteText * my_wt;
  ProgressWindow progresswindow;
  static int data_callback (void *userdata, int argc, char **argv, char **column_names);
  int on_data (int argc, char **argv);
};


#endif
