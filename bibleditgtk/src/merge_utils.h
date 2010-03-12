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

#ifndef INCLUDED_MERGE_UTILS_H
#define INCLUDED_MERGE_UTILS_H

#include "libraries.h"
#include <gtk/gtk.h>

ustring merge_conflict_markup(unsigned int number);

class MergeButton
{
public:
  MergeButton(int dummy);
  MergeButton();
  GtkTextChildAnchor * childanchor;
  GtkWidget * button;
  ustring text;
private:
};

class MergeButtonPair
{
public:
  MergeButtonPair(int dummy);
  MergeButtonPair();
  MergeButton button1;
  MergeButton button2;
private:
};

class Patch
{
public:
  Patch(int dummy);
  Patch();
  unsigned int linenumber;
  bool addition; // Whether addition or deletion.
  ustring change; // Change to apply.
private:
};

vector <Patch> merge_read_patch(const ustring& filename);

class ApproveButton
{
public:
  ApproveButton(int dummy);
  ApproveButton();
  GtkButton * button;
  Patch patch;
private:
};

const gchar * merge_new_line_indicator();
const gchar * merge_verse_indicator();
ustring merge_split_data(const ustring& data);
ustring merge_join_data(const ustring& data);
ustring merge_split_data_character(const ustring& data);
ustring merge_join_data_character(const ustring& data);

#endif
