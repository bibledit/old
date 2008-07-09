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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "text2pdf_utils.h"

#define CENTIMETERS_TO_INCHES_FACTOR 0.393700787 // 1 centimeter = 0.393700787 inches 
#define INCHES_TO_POINTS_FACTOR 72.0

int centimeters_to_pango_units(double centimeters) {
  int pango_units = int(centimeters * CENTIMETERS_TO_INCHES_FACTOR * INCHES_TO_POINTS_FACTOR * PANGO_SCALE);
  return pango_units;
}

int millimeters_to_pango_units(double millimeters) {
  int pango_units = int(millimeters / 10 * CENTIMETERS_TO_INCHES_FACTOR * INCHES_TO_POINTS_FACTOR * PANGO_SCALE);
  return pango_units;
}

double pango_units_to_millimeters (int pango_units)
{
  double millimeters = double(pango_units) / PANGO_SCALE / INCHES_TO_POINTS_FACTOR / CENTIMETERS_TO_INCHES_FACTOR * 10;
  return millimeters;
}

double pango_units_to_points (int pango_units)
{
  double points = double(pango_units) / PANGO_SCALE;
  return points;
}

PangoLayout * pango_layout_break_lines (cairo_t *cairo, int width_pango_units, const ustring& font, const ustring& text)
// Allocates a new PangoLayout that contains the "text" broken up in wrapped lines  
// that stay within the "width_pango_units".
// The object needs to be freed by the caller.
{
  PangoLayout * layout;
  layout = pango_cairo_create_layout(cairo);

  PangoContext * context = pango_layout_get_context(layout);
  pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

  PangoFontDescription *desc;
  desc = pango_font_description_from_string(font.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  pango_layout_set_width(layout, width_pango_units);
  pango_layout_set_indent(layout, millimeters_to_pango_units(0));
  pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
  pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
  pango_layout_set_auto_dir(layout, false);
  pango_layout_set_text(layout, text.c_str(), -1);

  return layout;
}
