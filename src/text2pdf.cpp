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
  pdffile = pdf_file;
  initialize_variables();

  // Create the cairo variables.
  surface = cairo_pdf_surface_create(pdffile.c_str(), pango_units_to_points(page_width_pango_units), pango_units_to_points(page_height_pango_units));
  cairo = cairo_create(surface);
}

Text2Pdf::~Text2Pdf()
// Object destructor.
{
  for (unsigned int i = 0; i < input_data.size(); i++) {
    delete input_data[i];
  }
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
  page_width_pango_units = millimeters_to_pango_units(210);
  page_height_pango_units= millimeters_to_pango_units(297);
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
  // The input variables.
  input_paragraph = NULL;
  // Layout engine.
  page = NULL;
  column = NULL;
  block = NULL;
  layoutcontainer = NULL;

  // Fonts.
  font = "Sans 12";
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
  // Close any open input containers.
  close_paragraph();

  // Go through the input data.
  run_input(input_data);

  // Print the pages.
  for (unsigned int pg = 0; pg < pages.size(); pg++) {
    T2PPage * page = pages[pg];
    page->print(cairo);
  }

  // Status information.
  status = cairo_surface_status(surface);
  if (status != CAIRO_STATUS_SUCCESS) {
    gw_warning(cairo_status_to_string(status));
  }
}

void Text2Pdf::run_input(vector <T2PInput *>& input)
// Goes through all of the input data.
{
  for (unsigned int i = 0; i < input.size(); i++) {
    switch (input[i]->type)
    {
      case tp2itParagraph:
      {
        input_paragraph = (T2PInputParagraph *) input[i];
        break;
      }
      case tp2itText:
      {
        T2PInputText * textinput = (T2PInputText *) input[i];
        lay_out_paragraph(textinput->text);
        break;
      }
    }
    run_input(input[i]->children);
  }
}

void Text2Pdf::lay_out_paragraph(const ustring& paragraph)
// Lays out one paragraph.
{
  ustring text(paragraph);
  unsigned int line_number = 0;
  while (!text.empty() && line_number < 1000) {
    get_next_layout_container();
    layoutcontainer->layout_text(font, input_paragraph, line_number, text);
    line_number++;
  }
  if (!text.empty()) {
    gw_warning("Can't fit in \"" + text + "\"");
  }
  fit_blocks_on_page();
}

void Text2Pdf::get_next_layout_container()
// Gets the next layout container to be used.
{
  // Create a page if there's none.
  if (page == NULL) {
    next_page ();
  }
  // Create a new column if there's none.
  if (column == NULL) {
    column = page->text_reference_area->next_column();
  }
  // Create a new block if needed.
  if (block == NULL || !block->layoutcontainers.empty()) {
    block = column->next_block();
  }
  // Create a new layout container.
  layoutcontainer = block->next_layout_container(cairo);
}

void Text2Pdf::fit_blocks_on_page ()
// Fits the block onto the current page, and creates new pages if need be.
{
  // Fit the blocks in the column, and gather any blocks that didn't fit.
  vector <T2PBlock *> non_fitting_blocks;
  column->fit_blocks(non_fitting_blocks);

  // Put any extra block on a new page.
  if (!non_fitting_blocks.empty ()) {
    next_page ();
    for (unsigned int i = 0; i < non_fitting_blocks.size(); i++) {
      column->add_block (non_fitting_blocks[i]);
    }
    fit_blocks_on_page();
  }
}

void Text2Pdf::next_page()
// Produces the next page and related objects.
{
  int page_number = pages.size() + 1;
  int width = page_width_pango_units;
  int height = page_height_pango_units;
  int inside_margin = inside_margin_pango_units;
  int outside_margin = outside_margin_pango_units;
  int top_margin = top_margin_pango_units;
  int bottom_margin = bottom_margin_pango_units;
  int header_height = header_height_pango_units;
  int footer_height = footer_height_pango_units;
  page = new T2PPage (page_number, width, height, inside_margin, outside_margin, top_margin, bottom_margin, header_height, footer_height);
  pages.push_back(page);
  column = page->text_reference_area->next_column();
}


void Text2Pdf::open_paragraph(int first_line_indent_mm)
/* 
 Open a new paragraph and add this to the input data.
 
 First line indent.
 
 */
{
  close_paragraph();
  input_paragraph = new T2PInputParagraph (first_line_indent_mm);
  input_data.push_back(input_paragraph);
}

void Text2Pdf::open_paragraph()
// Open a new paragraph and add this to the input data.
{
  close_paragraph();
  input_paragraph = new T2PInputParagraph ();
  input_data.push_back(input_paragraph);
}

void Text2Pdf::close_paragraph()
// Close the open paragraph.
{
  close_inline();
  input_paragraph = NULL;
}

void Text2Pdf::open_inline()
// Open a new inline container and add it to the input data.
{
  // If no paragraph is open, open it first, a default one.  
}

void Text2Pdf::close_inline()
// Close the open inline container.
{

}

void Text2Pdf::add_text(const ustring& text)
// Add text to whatever container is on top of the stack.
{
  T2PInputText * input_text = new T2PInputText (tp2itText, text);
  if (input_paragraph == NULL) {
    open_paragraph();
  }
  input_paragraph->children.push_back(input_text);
}

void Text2Pdf::view()
// View the pdf file.
{
  pdfviewer_view(pdffile);
}

void Text2Pdf::set_font(const ustring& font_in)
// Sets the font to use.
{
  font = font_in;
}

/*

 Todo text2pdf
 
 The PangoLayout is justified this way:
 Repeat measuring and adding tiny spaces in all white space, till it fills.
 We use the markup system, so that the space before/after white space characters is modified.
 If there are no white spaces, then we need to apply the markup to all characters of that one word.
  
 To implement:
 Left, right, full justified.
 Multiple paragraphs, causing a new page.  
 Multiple columns.
 Multiple pages.
 Column balancing.
 Remaining paragraph markup.
 Inline objects, fully.
 USFM to TextInput converter, for testing real-world examples.
 Headers
 Notes
 Drop-caps chapter numbers.
 Flowing notes.
 Mixture of normal and flowing notes, with the flowing ones at the bottom.
 
  The object receives other objects, such as paragraphs, etc, inline objects, and so on.
 
 A block can have one or more PangoLayouts. In the case that
 a PangoLayout intrudes in a block, then PangoLayouts are filled up till the height of the block has been reached,
 e.g. if chapter "10" is in a float, then this needs to ensure that the lines are at least two
 in that block.
 
 Paragraphs that keep with the next paragraph, as many as there can be, they all go in one block.
 
 Space before and after paragraphs are added manually using empty blocks with that space.
 At the start and end of the reference area, these empty blocks are disregarded.
 
 When images are rendered, these go into the LayoutContainer object, though the name may have to be changed at that stage.
 
 */

