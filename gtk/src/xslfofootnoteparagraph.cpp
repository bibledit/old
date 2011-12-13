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

#include "xslfofootnoteparagraph.h"
#include "usfmtools.h"

XslFoFootnoteParagraph::XslFoFootnoteParagraph(ustring marker_i, double fontsize_i, ustring italic_i, ustring bold_i, ustring underline_i, ustring smallcaps_i, ustring justification_i, double spacebefore_i, double spaceafter_i, double leftmargin_i, double rightmargin_i, double firstlineindent_i, bool apocrypha_i)
{
  marker_open = usfm_get_full_opening_marker(marker_i);
  marker_close = usfm_get_full_closing_marker(marker_i);
  fontsize = fontsize_i;
  italic = italic_i;
  bold = bold_i;
  underline = underline_i;
  smallcaps = smallcaps_i;
  justification = justification_i;
  spacebefore = spacebefore_i;
  spaceafter = spaceafter_i;
  leftmargin = leftmargin_i;
  rightmargin = rightmargin_i;
  firstlineindent = firstlineindent_i;
  apocrypha = apocrypha_i;
}

XslFoFootnoteParagraph::~XslFoFootnoteParagraph()
{
}
