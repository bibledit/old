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
#include "text2pdf_utils.h"

T2PLayoutContainer::T2PLayoutContainer(PangoRectangle rectangle_in, T2PArea * parent_in, cairo_t *cairo) :
  T2PArea(rectangle_in)
// This is a container for one PangoLayout.
{
  layout = pango_cairo_create_layout(cairo);
  parent = parent_in;
  pango_layout_set_width(layout, rectangle.width);
}

T2PLayoutContainer::~T2PLayoutContainer()
// Destructor.
{
  g_object_unref(layout);
}

void T2PLayoutContainer::layout_text(const ustring& font, T2PInputParagraph * paragraph, unsigned int line_number, ustring& text)
// This lays text out in the object, one line.
// The text to be laid out comes in "text".
// The remnant of the text, that didn't fit, is returned through "text". 
{
  // Text direction.
  PangoContext * context = pango_layout_get_context(layout);
  pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

  // Font.
  PangoFontDescription *desc;
  desc = pango_font_description_from_string(font.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  // First line indentation.
  if (line_number == 0) {
    if (paragraph->first_line_indent_mm > 0) {
      pango_layout_set_indent(layout, millimeters_to_pango_units(paragraph->first_line_indent_mm));
    }
    if (paragraph->first_line_indent_mm < 0) {
      int absolute_first_line_indent_pango_units = millimeters_to_pango_units(ABS(paragraph->first_line_indent_mm)); 
      rectangle.width += absolute_first_line_indent_pango_units;
      rectangle.x -= absolute_first_line_indent_pango_units;
    }
  }
  pango_layout_set_width(layout, rectangle.width);
  pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
  pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
  pango_layout_set_auto_dir(layout, false);
  pango_layout_set_text(layout, text.c_str(), -1);

  // Get the first line in the layout, and ensure that there's only one in the layout.
  // Store any remnant of the input text.
  PangoLayoutLine * layoutline = pango_layout_get_line(layout, 0);
  if (layoutline) {
    ustring line = text.substr(layoutline->start_index, layoutline->length);
    if (pango_layout_get_line_count(layout) != 1) {
      pango_layout_set_text(layout, line.c_str(), -1);
    }
    text.erase(0, line.length());
  } else {
    text.clear();
  }

  // Store the height of the layout.
  pango_layout_get_size(layout, NULL, &(rectangle.height));
  
  // Call the parent to refit the container in.
  ((T2PBlock *)parent)->refit_layout_container (this);
}

void T2PLayoutContainer::print(cairo_t *cairo)
// Print the content.
{
  // Write text in black.
  cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
  // Move into position.
  cairo_move_to(cairo, pango_units_to_points(rectangle.x), pango_units_to_points(rectangle.y));
  // Show text.
  if (layout) {
    pango_cairo_show_layout(cairo, layout);
  }
}
