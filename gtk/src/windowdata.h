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


#ifndef INCLUDED_WINDOWDATA_H
#define INCLUDED_WINDOWDATA_H


#include "libraries.h"
#include <gtk/gtk.h>


enum WindowID { widShowRelatedVerses, widMerge, widResource, 
                widOutline, widCheckKeyterms, widStyles, widNotes, widReferences,
                widEditor, widMenu, widShowVerses, widCheckUSFM, widSourceLanguages };


class WindowData
{
public:
  WindowData(bool save_on_destroy);
  ~WindowData();
  vector<int> widths;
  vector<int> heights;
  vector<int> x_positions;
  vector<int> y_positions;
  vector<int> ids;
  vector<ustring> titles;
  vector<bool> shows;
  void debug();
private:
  bool my_save_on_destroy;
};


#endif
