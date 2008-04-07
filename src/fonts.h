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


#ifndef INCLUDED_FONTUTILS_H
#define INCLUDED_FONTUTILS_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


enum FontVariant {fvRegular, fvItalic, fvBold, fvBoldItalic};


bool font_get_afm_file (ustring& file);
vector<ustring> font_get_sizes ();


class PrintingFonts
{
public:
  PrintingFonts (const ustring& project);
  vector <ustring> xep_families;
  vector <ustring> xep_paths;
  vector <ustring> xep_metrics;
  vector <FontVariant> xep_variants;
  vector <ustring> available_families;
  vector <ustring> printing_families;
  vector <ustring> sizes;
  void printing_families_store (const vector<ustring>& fonts);
  ustring printing_families_comma_separated ();
private:
  set <ustring> families_set;
  void ensure_font_available (const ustring& font);
};


#endif
