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

void Text2Pdf::run() // Todo working here on implementing each object's own print function.
// Runs the converter.
{
  // Close any open input containers.
  close_paragraph();

  // Go through the input data.
  run_input(input_data);

  // Make the background white.
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_paint(cairo);

  for (unsigned int pg = 0; pg < pages.size(); pg++) {
    T2PReferenceArea * text_reference_area = pages[pg]->text_reference_area;
    cout << "Text reference area rectangle " << text_reference_area->rectangle2text() << endl; // Todo
    for (unsigned int col = 0; col < text_reference_area->columns.size(); col++) {
      T2PColumn * column = (T2PColumn *)text_reference_area->columns[col];
      cout << "Column rectangle " << column->rectangle2text() << endl; // Todo
      for (unsigned int blk = 0; blk < column->blocks.size(); blk++) {
        T2PBlock * block = column->blocks[blk];
        cout << "Block rectangle " << block->rectangle2text() << endl; // Todo
        for (unsigned int l = 0; l < block->layouts.size(); l++) {
          T2PLayoutContainer * layoutcontainer = block->layouts[l];
          cout << "LayoutContainer rectangle " << layoutcontainer->rectangle2text() << endl; // Todo
          layoutcontainer->print (cairo, block->rectangle.x, block->rectangle.y);
        }
      }
    }
  }

  // Output the page.
  cairo_show_page(cairo);

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
    if (input[i]->type == tp2itText) {
      T2PInputText * textinput = (T2PInputText *) input[i];
      lay_out_text(textinput->text);
      fit_blocks_in_column();
    }
    run_input(input[i]->children);
  }
}

void Text2Pdf::lay_out_text(const ustring& text)
// Lays out text.
{
  // Break the lines so that they stay within the width.
  vector <ustring> texts;
  int wrap_width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
  PangoLayout * wrap_layout = pango_layout_break_lines(cairo, wrap_width, font, text);
  for (int i = 0; i < pango_layout_get_line_count(wrap_layout); i++) {
    PangoLayoutLine * layoutline = pango_layout_get_line(wrap_layout, i);
    if (layoutline) {
      texts.push_back(text.substr(layoutline->start_index, layoutline->length));
    }
  }
  g_object_unref(wrap_layout);

  // Go through the lines found.
  for (unsigned int t = 0; t < texts.size(); t++) {

    // Create a Block with appropriate width.
    PangoRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
    rectangle.height = 0;
    T2PBlock * block;
    block = new T2PBlock (rectangle);
    blocks.push_back(block);

    // Create a layout and add it to the block.
    PangoLayout *layout;
    layout = pango_cairo_create_layout(cairo);
    T2PLayoutContainer * layoutcontainer = new T2PLayoutContainer (rectangle, layout);
    block->layouts.push_back(layoutcontainer);

    PangoContext * context = pango_layout_get_context(layout);
    pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

    PangoFontDescription *desc;
    desc = pango_font_description_from_string(font.c_str());
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    pango_layout_set_width(layout, block->rectangle.width);
    //pango_layout_set_indent (layout, millimeters_to_pango_units(5));
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    pango_layout_set_auto_dir(layout, false);
    pango_layout_set_text(layout, texts[t].c_str(), -1);

    pango_layout_get_size(layout, NULL, &(block->rectangle.height)); // Todo set the height of the layout container, and block depends on that.

  }
}

void Text2Pdf::fit_blocks_in_column()
// Fits blocks in a column;
{
  // Create a page if there's none.
  if (page == NULL) {
    page
        = new T2PPage (pages.size() + 1, page_width_pango_units, page_height_pango_units, inside_margin_pango_units, outside_margin_pango_units, top_margin_pango_units, bottom_margin_pango_units, header_height_pango_units, footer_height_pango_units);
    pages.push_back(page);
    column = page->text_reference_area->next_column();
  }
  for (unsigned int i = 0; i < blocks.size(); i++) {
    column->add_block(blocks[i]);
  }
  blocks.clear();
}

void Text2Pdf::open_paragraph()
// Open a new paragraph and add this to the input data.
{
  close_paragraph();
  input_paragraph = new T2PInput (tp2itParagraph);
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

 The x/y positions of rectangles must be redone,so that they point to their offset in their parents,
 not relative to the page.
 Each layout must have positions too, so that means that each layout object, and only one, will go 
 into an encapsulating object. Also when images are rendered, these go into the same encapsulated object.
 Each object does it own rendering, having a .print() member.
 Each object does its own laying out, e.g. the LayoutContainer contains all code needed to create its own layout.
 Each object stores its parent, and has a parent() function to store that parent in case of reparenting when pouring 
 the content in the page. Using this parent it can find its own offsets when printing.
 Todo working on implementing the above.

 The object receives other objects, such as paragraphs, etc, inline objects, and so on.
 
 To implement:
 Paragraph indentation.
 Multiple columns.
 Multiple pages.
 Column balancing.
 Remaining paragraph markup.
 Inline objects, fully.
 
 
 
 The PangoLayout is justified this way:
 Load whole paragraph, set wrapping on, set size.
 Retrieve first line, if there's any.
 Set size off, set wrapping off, load that first line
 Repeat measuring and adding tiny spaces in all white space, till it fills.
 Fine tune with kerning, that is, the space between characters.
 
 A column has blocks, and these blocks can be shifted when balancing the column.
 
 A block can have one or more PangoLayouts. In the case that
 a PangoLayout intrudes in a block, then PangoLayouts are filled up till the height of the block has been reached,
 e.g. if chapter "10" is in a float, then this needs to ensure that the lines are at least two
 in that block.
 
 Paragraphs that keep with the next paragraph, as many as there can be, they all go in one block.
 
 Space before and after paragraphs are added manually using empty blocks with that space.
 At the start and end of the reference area, these empty blocks are disregarded.
 
 
 */

