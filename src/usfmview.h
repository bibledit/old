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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_USFM_VIEW_H
#define INCLUDED_USFM_VIEW_H

#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "style.h"
#include "types.h"
#include "editor_aids.h"
#include "highlight.h"
#include "spelling.h"
#include <gtksourceview/gtksourceview.h>

class USFMView
{
public:
  USFMView(GtkWidget * vbox, const ustring& project_in);
  ~USFMView();
  Reference current_reference;
  void chapter_load(unsigned int chapter_in, vector <ustring> * lines_in = NULL);
  void book_set(unsigned int book_in);
  void chapter_save();
  ustring project;
private:
  GtkWidget *scrolledwindow;
  GtkSourceBuffer * sourcebuffer;
  GtkWidget * sourceview;
  unsigned int book;
  unsigned int chapter;
  unsigned int reload_chapter_number;
  ustring get_chapter();
};


#endif
