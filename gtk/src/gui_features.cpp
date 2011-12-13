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

#include "gui_features.h"
#include "settings.h"
#include "utilities.h"

GuiFeatures::GuiFeatures(int dummy)
{
  // Get features variables.
  extern Settings *settings;
  ustring pattern = settings->genconfig.features_list_get();

  b_basics = settings->genconfig.features_mode_get() == 0;
  b_full = settings->genconfig.features_mode_get() == 1;

  // The order of this pattern handling should agree to the order used in the features dialog source.
  b_printing = bitpattern_take(pattern);
  b_project_management = bitpattern_take(pattern);
  b_references_management = bitpattern_take(pattern);
  b_styles = bitpattern_take(pattern);
  b_styles_management = bitpattern_take(pattern);
  b_project_notes = bitpattern_take(pattern);
  b_project_notes_management = bitpattern_take(pattern);
  b_references_and_find = bitpattern_take(pattern);
  b_replace = bitpattern_take(pattern);
  b_checks = bitpattern_take(pattern);
  b_tools = bitpattern_take(pattern);
  b_preferences = bitpattern_take(pattern);
}

bool GuiFeatures::printing()
{
  return !b_basics && (b_full || b_printing);
}

bool GuiFeatures::project_management()
{
  return !b_basics && (b_full || b_project_management);
}

bool GuiFeatures::references_management()
{
  return !b_basics && (b_full || b_references_management);
}

bool GuiFeatures::styles()
{
  return !b_basics && (b_full || b_styles);
}

bool GuiFeatures::styles_management()
{
  return !b_basics && (b_full || b_styles_management);
}

bool GuiFeatures::project_notes()
{
  return !b_basics && (b_full || b_project_notes);
}

bool GuiFeatures::project_notes_management()
{
  return !b_basics && (b_full || b_project_notes_management);
}

bool GuiFeatures::references_and_find()
{
  return !b_basics && (b_full || b_references_and_find);
}

bool GuiFeatures::replace()
{
  return !b_basics && (b_full || b_replace);
}

bool GuiFeatures::checks()
{
  return !b_basics && (b_full || b_checks);
}

bool GuiFeatures::tools()
{
  return !b_basics && (b_full || b_tools);
}

bool GuiFeatures::preferences()
{
  return !b_basics && (b_full || b_preferences);
}
