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

#ifndef INCLUDED_USFM2OSIS_H
#define INCLUDED_USFM2OSIS_H

#include "libraries.h"
#include <libxml/xmlwriter.h>
#include "usfm2xslfo_utils.h"
#include "progresswindow.h"
#include "notecaller.h"
#include "portion_utils.h"
#include "localizednumerals.h"

class Usfm2Osis
{
public:
  Usfm2Osis(const ustring& file);
  ~Usfm2Osis();
  void set_stylesheet (const ustring& name);
  void header (const ustring& name, const ustring& description);
  void open_book(unsigned int id);
  void close_book();
  void load_book (vector <ustring>& data);
private:
  ustring osisfile;
  ustring stylesheet;
  xmlBufferPtr xmlbuffer;
  xmlTextWriterPtr xmlwriter;
  unsigned int book_bibledit_id;
  ustring book_osis_id;
  void transform_headers_and_descriptions(ustring& usfmcode);
  void transform_per_osis_division (ustring& usfmcode);
  bool usfm_is_osis_division (const ustring& marker);
  void transform_block(ustring& usfm_code);
  void transform_fallback(ustring& usfm_code);
  void transform_remove_marker (ustring& usfm_code, size_t marker_length);
  void transform_usfm_description (ustring& usfm_code, const ustring& marker_text, size_t marker_length);

  void transform_h_title (ustring& usfm_code, size_t marker_length, bool runningheader, const gchar * placement);
  void transform_general_title (ustring& usfm_code, size_t marker_length, const gchar * type, unsigned int level);

  void transform_division (const gchar * type, bool canonical);
  bool division_open;
  void ensure_division_opened ();
  void ensure_division_closed ();

  void transform_paragraph_start (ustring& usfm_code, size_t marker_length);
  bool paragraph_open;
  void ensure_paragraph_opened ();
  void ensure_paragraph_closed ();
  
  unsigned int chapter_number;
  ustring chapter_osis_id;
  void ensure_chapter_closed ();
  void transform_chapter_number (ustring& usfm_code, size_t marker_length);

  unsigned int verse_number;
  ustring verse_osis_id;
  void ensure_verse_closed ();
  void transform_verse_number (ustring& usfm_code, size_t marker_length);

  void transform_note (ustring& usfm_code, size_t marker_length, bool is_opener, bool endnote, bool xref);
  bool note_is_open;
  void ensure_note_closed ();
  
  bool character_style_open;
  void transform_character_style (ustring& usfm_code, size_t marker_length, bool is_opener, const gchar * element);
  void transform_character_style (ustring& usfm_code, size_t marker_length, bool is_opener, const gchar * element, const gchar * attribute_name, const gchar * attribute_value);
  void transform_character_style (ustring& usfm_code, size_t marker_length, bool is_opener, const gchar * element, const gchar * attribute1_name, const gchar * attribute1_value, const gchar * attribute2_name, const gchar * attribute2_value);
  void ensure_character_style_closed();
  
  void transform_figure (ustring& usfm_code, const ustring& marker_text, size_t marker_length);
};

#endif
