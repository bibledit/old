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

#include "text2pdf_area.h"
#include "text2pdf_utils.h"
#include "text2pdf_ref_area.h"
#include "text2pdf_layout.h"

T2PLayoutContainer::T2PLayoutContainer(PangoRectangle initial_rectangle, PangoLayout * layout_in) :
  T2PArea(initial_rectangle)
// This is a container for one PangoLayout.
{
  layout = layout_in;
}

T2PLayoutContainer::~T2PLayoutContainer()
// Destructor.
{
  // Free the layout object.
  if (layout) {
    g_object_unref(layout);
  }
}

void T2PLayoutContainer::print(cairo_t *cairo, int x_pango_units, int y_pango_units)
// Print the content.
{
  // Write text in black.
  cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
  // Move into position.
  cairo_move_to(cairo, pango_units_to_points(x_pango_units), pango_units_to_points(y_pango_units));
  // Show text.
  if (layout) {
    pango_cairo_show_layout(cairo, layout);
  }
}
