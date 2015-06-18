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

#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "style.h"
#include "xmlutils.h"
#include "gwrappers.h"
#include "directories.h"
#include "stylesheetutils.h"
#include "constants.h"
#include <glib/gi18n.h>

Style::Style(const ustring & stylesheet, const ustring & style, bool write)
// Reads a style from the database. Deprecated, use StyleV2 instead for new code.
{
  // Save variables.
  mystylesheet = stylesheet;
  marker = style;
  mywrite = write;
  // Set default values.
  name = _("Marker");
  info = _("Unified Standard Format Marker");
  fontsize = 12;
  italic = OFF;
  bold = OFF;
  underline = OFF;
  underline = OFF;
  superscript = false;
  justification = LEFT;
  spacebefore = 0;
  spaceafter = 0;
  leftmargin = 0;
  rightmargin = 0;
  firstlineindent = 0;
  spancolumns = false;
  type = stInlineText;
  subtype = 0;
  color = 0;
  print = true;
  userbool1 = false;
  userbool2 = false;
  userbool3 = false;
  userint1 = 0;
  userint2 = 0;
  userint3 = 0;
  // Read values from the database.
  stylesheet_load_style(mystylesheet, *this);
}

Style::~Style()
{
  // Save style in database.
  if (mywrite) {
    stylesheet_save_style(mystylesheet, *this);
  }
}


StyleV2::StyleV2(int dummy)
// Contains the values for the style of one marker.
{
  // Set default values.
  name = _("Marker");
  info = _("Unified Standard Format Marker");
  fontsize = 12;
  italic = OFF;
  bold = OFF;
  underline = OFF;
  underline = OFF;
  superscript = false;
  justification = LEFT;
  spacebefore = 0;
  spaceafter = 0;
  leftmargin = 0;
  rightmargin = 0;
  firstlineindent = 0;
  spancolumns = false;
  type = stInlineText;
  subtype = 0;
  color = 0;
  print = true;
  userbool1 = false;
  userbool2 = false;
  userbool3 = false;
  userint1 = 0;
  userint2 = 0;
  userint3 = 0;
}


StyleV2::~StyleV2()
{
}


