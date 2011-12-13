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


#ifndef INCLUDED_NOTE_TEMPLATE_H
#define INCLUDED_NOTE_TEMPLATE_H


#include "libraries.h"
#include "types.h"


class NoteTemplate
// Manages settings for one note template.
{
public:
  NoteTemplate (const ustring& filename, NoteType notetype, bool write);
  ~NoteTemplate ();
  int numbering;
  ustring anchor;
  bool automatic_reference;
  bool reference_chapter;
  ustring reference_division;
  bool reference_verse;
  ustring reference_suffix;
  vector<ustring> content_markers;
  vector<ustring> content_texts;
  vector<bool> content_remembers;
  vector<gint> content_sizes;
  vector<bool> content_tidies;
private:
  void load ();
  ustring myfilename;
  bool mywrite;
  ustring current_element;
  ustring current_value;
  ustring content_marker;
  ustring content_text;
  ustring content_remember;
  ustring content_tidy;
  ustring content_size;
  static void start_element_handler  (GMarkupParseContext *context,
                                      const gchar         *element_name,
                                      const gchar        **attribute_names,
                                      const gchar        **attribute_values,
                                      gpointer             user_data,
                                      GError             **error);
  void start_element_handler (const ustring& element_name);
  static void end_element_handler    (GMarkupParseContext *context,
                                      const gchar         *element_name,
                                      gpointer             user_data,
                                      GError             **error);
  void end_element_handler   (const ustring& element_name);
  static void text_handler           (GMarkupParseContext *context,
                                      const gchar         *text,
                                      gsize                text_len,
                                      gpointer             user_data,
                                      GError             **error);
  void text_handler          (const ustring& text);
  static void passthrough_handler    (GMarkupParseContext *context,
                                      const gchar         *passthrough_text,
                                      gsize                text_len,
                                      gpointer             user_data,
                                      GError             **error);
  static void error_handler          (GMarkupParseContext *context,
                                      GError              *error,
                                      gpointer             user_data);
};


#endif
