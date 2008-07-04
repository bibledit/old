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

#include "text2pdf.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include <glib.h>
#include "utilities.h"
#include "tiny_utilities.h"
#include "date_time_utils.h"
#include "usfmtools.h"
#include "constants.h"
#include "books.h"
#include "color.h"
#include "directories.h"
#include "xep.h"
#include "gtkwrappers.h"
#include "pdfviewer.h"
#include "settings.h"
#include "referenceutils.h"
#include "projectutils.h"
#include <math.h>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>

Text2Pdf::Text2Pdf(const ustring& pdf_file)
// Converts text code to a pdf file.
{
  // Initializers.
  initialize_variables();

  // Create the cairo variables.
  surface = cairo_pdf_surface_create(pdf_file.c_str(), pango_units_to_points(page_width_pango_units), pango_units_to_points(page_height_pango_units));
  cairo = cairo_create(surface);
}

Text2Pdf::~Text2Pdf()
// Object destructor.
{
  for (unsigned int i = 0; i < pages.size(); i++) {
    delete pages[i];
  }
  cairo_surface_destroy(surface);
  cairo_destroy(cairo);
}

void Text2Pdf::initialize_variables()
// Initialize the variables for the object.
{
  // The page size defaults to A4 = 210 x 297 millimeters (8.27 x 11.69 inches).
  page_width_pango_units = millimeters_to_pango_units (210);
  page_height_pango_units= millimeters_to_pango_units (297);
  // Ppage margins default in centimeters.
  inside_margin_pango_units = centimeters_to_pango_units(2.5);
  outside_margin_pango_units = centimeters_to_pango_units(1.5);
  top_margin_pango_units = centimeters_to_pango_units(2);
  bottom_margin_pango_units = centimeters_to_pango_units(2);
  // The height of the header default value in centimeters.  
  header_height_pango_units = centimeters_to_pango_units(1);
  // The height of the footer default value in centimeters.
  footer_height_pango_units = centimeters_to_pango_units(0);
  // The default spacing between the two columsn in centimeters.
  column_spacing_pango_units = centimeters_to_pango_units(0.5);
}

void Text2Pdf::page_size_set(double width_centimeters, double height_centimeters)
// Set the page size in centimeters.
{
  page_width_pango_units = centimeters_to_pango_units(width_centimeters);
  page_height_pango_units = centimeters_to_pango_units(height_centimeters);
  cairo_pdf_surface_set_size(surface, pango_units_to_points(page_width_pango_units), pango_units_to_points(page_height_pango_units));
}

void Text2Pdf::page_margins_set(double inside_margin_centimeters, double right_margin_centimeters, double top_margin_centimeters, double bottom_margin_centimeters)
// Set the margins of the page, in centimeters.
{
  inside_margin_pango_units = centimeters_to_pango_units(inside_margin_centimeters);
  outside_margin_pango_units = centimeters_to_pango_units(right_margin_centimeters);
  top_margin_pango_units = centimeters_to_pango_units(top_margin_centimeters);
  bottom_margin_pango_units = centimeters_to_pango_units(bottom_margin_centimeters);
}

void Text2Pdf::header_height_set(double size_centimeters)
// Set the height of the header.
{
  header_height_pango_units = centimeters_to_pango_units(size_centimeters);
}

void Text2Pdf::footer_height_set(double size_centimeters)
// Set the height of the footer.
{
  footer_height_pango_units = centimeters_to_pango_units(size_centimeters);
}

void Text2Pdf::column_spacing_set(double spacing_centimeters)
// Sets the spacing between the two columns.
// Text is normally laid out in two columns, but there are elements that span the columns.
{
  column_spacing_pango_units = centimeters_to_pango_units(spacing_centimeters);
}

void Text2Pdf::run()
// Runs the converter.
{
  // Make the background white.
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_paint(cairo);

  // Create a page
  T2PPage * page = new T2PPage (pages.size() + 1, page_width_pango_units, page_height_pango_units, inside_margin_pango_units, outside_margin_pango_units, top_margin_pango_units, bottom_margin_pango_units, header_height_pango_units, footer_height_pango_units);
  pages.push_back (page);
  
  PangoLayout *layout;
  layout = pango_cairo_create_layout(cairo);

  PangoContext * context = pango_layout_get_context(layout);
  pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

  PangoFontDescription *desc;
  desc = pango_font_description_from_string("Sans 16");
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  ustring text = "I love you because we follow the truth, dear friend, and I pray that all goes well for you. I hope that you are as strong in body, as I know you are in spirit. It makes me very happy when the Lord's followers come by and speak openly of how you obey the truth. Nothing brings me greater happiness than to hear that my children are obeying the truth.";
  
  pango_layout_set_width(layout, pages[0]->text_reference_area->rectangle.width);
  //pango_layout_set_indent (layout, millimeters_to_pango_units(5));
  pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
  pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
  pango_layout_set_auto_dir(layout, false);
  pango_layout_set_text(layout, text.c_str(), -1);

  for (int i = 0; i < pango_layout_get_line_count (layout); i++) {
    PangoLayoutLine * layoutline = pango_layout_get_line (layout, i);
    if (layoutline) {
      cout << text.substr (layoutline->start_index, layoutline->length) << endl; // Todo
    }
  }
  
  int layout_width, layout_height;
  pango_layout_get_size(layout, &layout_width, &layout_height);

  // Write text in black.
  cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
  
  cairo_move_to(cairo, pango_units_to_points(pages[0]->text_reference_area->rectangle.x), pango_units_to_points(pages[0]->text_reference_area->rectangle.y));
  pango_cairo_show_layout(cairo, layout);

  // Free the layout object
  g_object_unref(layout);

  // Output the page.
  cairo_show_page(cairo);
  
  // Status information.
  status = cairo_surface_status(surface);
  if (status != CAIRO_STATUS_SUCCESS) {
    gw_warning(cairo_status_to_string(status));
  }
}

/*

 Todo text2pdf
 
 We use area objects here, and stacking, and parents and children, 
 so that the layout is done in memory, then written to pdf at the very end.
 Labels should receive values as well.

 Not to call this usfm2pdf, but text2pdf, so that this means that no 
 knowledge of any usfm is needed in the object. The transformation from usfm to text 
 is done in another object. 
 
 The object received otherobjects, such as paragraphs, etc, inline objects, and so on.
 
 The PangoLayout is justified this way:
 Load whole paragraph, set wrapping on, set site.
 Retrieve first line, if there's any.
 Set size off, set wrapping off, load that first line
 Repeat measuring and adding tiny spaces in all white space, till it fills.
 Finetyne with kerning, that is, the space between characters.
 
 A column has blocks, and these blocks can be shifted when balancing the column.
 
 A block can have one or more blocks again, or only inline objects. In the case that
 a block intrudes in a block, then blocks are filled up till the height of the block has been reached,
 e.g. if chapter "10" is in a float, then this needs to ensure that the lines are at least two
 in that block.
 
 Paragraphs that keep with the next paragraph, as many as there can be, they all go in one block.
  
 We won't use inline objects, but instead the PangoLayout fills the whole block.
 There's space for intrusion, which is a PangoLayout again.
 
 Space before and after paragraphs are added manually using empty blocks with that space.
 At the start and end of the reference area, these empty blocks are disregarded.
 
 
 */

