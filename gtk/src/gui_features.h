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

#ifndef INCLUDED_GUI_FEATURES_H
#define INCLUDED_GUI_FEATURES_H

#include "libraries.h"

class GuiFeatures
{
public:
  GuiFeatures(int dummy);
  bool printing();
  bool project_management();
  bool references_management();
  bool styles();
  bool styles_management();
  bool project_notes();
  bool project_notes_management();
  bool references_and_find();
  bool replace();
  bool checks();
  bool tools();
  bool preferences();
private:
  bool b_basics;
  bool b_full;
  bool b_printing;
  bool b_project_management;
  bool b_references_management;
  bool b_styles;
  bool b_styles_management;
  bool b_project_notes;
  bool b_project_notes_management;
  bool b_references_and_find;
  bool b_replace;
  bool b_checks;
  bool b_tools;
  bool b_preferences;
};

#endif
