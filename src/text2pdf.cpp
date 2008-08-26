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
  for (unsigned int i = 0; i < text_input_data.size(); i++) {
    delete text_input_data[i];
  }
  for (unsigned int i = 0; i < pages.size(); i++) {
    delete pages[i];
  }
  cairo_surface_destroy(surface);
  cairo_destroy(cairo);
  if (progresswindow)
    delete progresswindow;
}

void Text2Pdf::initialize_variables()
// Initialize the variables for the object.
{
  // The page size defaults to A4 = 210 x 297 millimeters (8.27 x 11.69 inches).
  page_width_pango_units = millimeters_to_pango_units(210);
  page_height_pango_units= millimeters_to_pango_units(297);
  // Page margins default in centimeters.
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
  // Whether one column only.
  one_column_only = false;
  // The input variables.
  input_paragraph = NULL;
  stacked_input_paragraph = NULL;
  keep_data_together = false;
  line_spacing = 100;
  right_to_left = false;
  print_date = false;

  // Layout engine.
  page = NULL;
  layoutcontainer = NULL;

  // Progress.
  progresswindow = NULL;
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

void Text2Pdf::page_one_column_only()
// Sets that there's only one column on the pages.
{
  one_column_only = true;
}

void Text2Pdf::run()
// Runs the converter.
{
  // Create progress window.
  if (progresswindow == NULL)
    progresswindow = new ProgressWindow ("Formatting text", false);

  // Close any open input containers.
  close_paragraph();

  // Go through the input data.
  run_input(text_input_data);

  // Find potential widows and orphans.
  find_potential_widows_and_orphans();

  // Fit the blocks on the pages.
  fit_blocks_on_pages();

  // Print the pages.
  progresswindow->set_iterate(0.5, 1, pages.size());
  for (unsigned int pg = 0; pg < pages.size(); pg++) {
    progresswindow->iterate();
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
  progresswindow->set_iterate(0, 0.5, input.size());
  for (unsigned int i = 0; i < input.size(); i++) {
    progresswindow->iterate();
    switch (input[i]->type)
    {
      case t2pitParagraph:
      {
        input_paragraph = (T2PInputParagraph *) input[i];
        lay_out_paragraph();
        break;
      }
      case t2pitOpenKeepTogether:
      {
        // Paragraphs from here till the next t2pitCloseKeepTogether should be kept together
        // in one column or page.
        // Set the flag.
        keep_data_together = true;
        break;
      }
      case t2pitCloseKeepTogether:
      {
        // Clear the flag.
        keep_data_together = false;
        // To properly work with the layout engine's model, add an empty block.
        PangoRectangle rectangle;
        rectangle.x = 0;
        rectangle.y = 0;
        rectangle.width = 0;
        rectangle.height = 0;
        T2PBlock * block = new T2PBlock (rectangle, 1);
        block->type = t2pbtSpaceAfterParagraph;
        text_input_blocks.push_back(block);
        break;
      }
    }
  }
}

void Text2Pdf::lay_out_paragraph()
// Lays out one paragraph.
{
  // Bail out if there's no text.
  if (input_paragraph->text.empty()) {
    return;
  }

  // Optionally lay out the paragraph's intrusion.
  T2PInputParagraph * intrusion = input_paragraph->get_intrusion();
  if (intrusion) {
    get_next_layout_container(true);
    layoutcontainer->layout_drop_caps(intrusion, input_paragraph->font_size_points);
  }

  // Space before paragraph.
  if (input_paragraph->space_before_mm != 0) {
    PangoRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
    rectangle.height = millimeters_to_pango_units(input_paragraph->space_before_mm);
    int column_count = get_column_count_rectangle_width(input_paragraph->column_count, rectangle.width);
    T2PBlock * block = new T2PBlock (rectangle, column_count);
    block->type = t2pbtSpaceBeforeParagraph;
    block->keep_with_next = true;
    text_input_blocks.push_back(block);
  }

  // Paragraph itself.
  unsigned int line_number = 0;
  while (!input_paragraph->text.empty() && line_number < 1000) {
    get_next_layout_container(false);
    layoutcontainer->layout_text(input_paragraph, line_number, input_paragraph->text);
    line_number++;
  }
  if (!input_paragraph->text.empty()) {
    gw_warning("Can't fit in \"" + input_paragraph->text + "\"");
  }

  // Space after paragraph.
  if (input_paragraph->space_after_mm != 0) {
    PangoRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
    rectangle.height = millimeters_to_pango_units(input_paragraph->space_after_mm);
    int column_count = get_column_count_rectangle_width(input_paragraph->column_count, rectangle.width);
    T2PBlock * block = new T2PBlock (rectangle, column_count);
    block->type = t2pbtSpaceAfterParagraph;
    block->keep_with_next = input_paragraph->keep_with_next;
    text_input_blocks.push_back(block);
  }
}

void Text2Pdf::get_next_layout_container(bool intrusion)
// Gets the next layout container to be used.
{
  // Rectangle for next block.
  PangoRectangle rectangle;
  rectangle.x = 0;
  rectangle.y = 0;
  // Only the width of the block is set, because the other sizes depend on factors not yet known at this stage.
  rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
  rectangle.height = 0;

  // Column count. If there's more than one, the width gets smaller.
  int column_count = get_column_count_rectangle_width(input_paragraph->column_count, rectangle.width);

  // Create a new block.
  T2PBlock * block = new T2PBlock (rectangle, column_count);
  block->keep_with_next = input_paragraph->keep_with_next || keep_data_together || intrusion;
  if (intrusion) {
    block->type = t2pbtTextIntrusion;
  }

  // Handle a preceding intrusion.
  if (!text_input_blocks.empty()) {
    unsigned int preceding_block_to = text_input_blocks.size();
    int preceding_block_from = preceding_block_to;
    // Look 2 blocks back, because by default the intrusion, that's the drop-caps, is two lines high.
    preceding_block_from -= 2;
    if (preceding_block_from < 0)
      preceding_block_from = 0;
    for (unsigned int i = preceding_block_from; i < preceding_block_to; i++) {
      if (text_input_blocks[i]->type == t2pbtTextIntrusion) {
        // Modify size and position of the block.
        block->rectangle.x += text_input_blocks[i]->rectangle.width;
        block->rectangle.width -= text_input_blocks[i]->rectangle.width;
        // Block stays with the next one.
        block->keep_with_next = true;
      }
    }
  }

  // Store the block.
  text_input_blocks.push_back(block);

  // Create a new layout container.
  layoutcontainer = block->next_layout_container(cairo);
}

void Text2Pdf::fit_blocks_on_pages()
// Fits the block onto the pages, and creates new pages if need be.
{
  // Loop through the input blocks.
  unsigned int infinite_loop_counter = 0;
  while (!text_input_blocks.empty() && infinite_loop_counter < 5000) {
    // Create another page.
    next_page();
    // Progress.
    progresswindow->pulse();
    progresswindow->set_text("Page " + convert_to_string(pages.size()));
    // Lay out the input on the page.
    page->text_reference_area->fit_blocks(text_input_blocks, column_spacing_pango_units);
  }
}

void Text2Pdf::find_potential_widows_and_orphans()
/*
 Widows are the first lines of a paragraph, while the rest is in another column or on another page.
 Orphans are the last lines of a pragraph in such a situation.
 We set two widows and two orphans.
 The first two lines of a paragraph could be widows.
 The last two lines of a paragraph could be orphans.
 It is implemented throught the "keep_with_next" property of the T2PBlock object.
 The "keep with next" mechanism will then take over when the blocks are fitted in.
 */
{
  for (unsigned int blk = 0; blk < text_input_blocks.size(); blk++) {
    T2PBlock * block = text_input_blocks[blk];
    // The first line of the paragraph is to be kept with the next.
    if (block->type == t2pbtTextParagraphFirstLine) {
      block->keep_with_next = true;
    }
    // The last-but-one line of the paragraph is to be kept with the next.
    if (block->type == t2pbtTextParagraphLastLine) {
      if (blk) {
        text_input_blocks[blk-1]->keep_with_next = true;
      }
    }
  }
}

int Text2Pdf::get_column_count_rectangle_width(int column_count_in, int& width) {
  int column_count = one_column_only ? 1 : column_count_in;
  if (column_count > 1) {
    width /= column_count;
    width -= (column_count - 1) * column_spacing_pango_units;
  }
  return column_count;
}

void Text2Pdf::next_page()
// Produces the next page and related objects.
{
  int page_number = pages.size() + 1;
  page = new T2PPage (page_number,
      page_width_pango_units, page_height_pango_units,
      inside_margin_pango_units, outside_margin_pango_units, top_margin_pango_units, bottom_margin_pango_units,
      header_height_pango_units, footer_height_pango_units, cairo, print_date);
  pages.push_back(page);
}

void Text2Pdf::open_keep_together()
// Anything between this and the closer is kept together in one page or column.
{
  close_paragraph();
  text_input_data.push_back(new T2PInput (t2pitOpenKeepTogether));
}

void Text2Pdf::close_keep_together()
// Close keeping together in one page or column.
{
  close_paragraph();
  text_input_data.push_back(new T2PInput (t2pitCloseKeepTogether));
}

void Text2Pdf::open_paragraph()
// Open a new paragraph and add this to the input data.
{
  close_paragraph();
  input_paragraph = new T2PInputParagraph (font, line_spacing, right_to_left);
  text_input_data.push_back(input_paragraph);
}

void Text2Pdf::ensure_open_paragraph()
// Ensures that a paragraph is open.
{
  if (input_paragraph == NULL)
    open_paragraph();
}

void Text2Pdf::paragraph_set_font_size(double points) 
// Sets the font size to be used in the paragraph, in points.
{
  ensure_open_paragraph();
  input_paragraph->font_size_points = points;
}

void Text2Pdf::paragraph_set_italic(bool italic)
// Set whether font should be italic.
{
  ensure_open_paragraph();
  input_paragraph->italic = italic;
}

void Text2Pdf::paragraph_set_bold(bool bold)
// Set whether font should be in bold.
{
  ensure_open_paragraph();
  input_paragraph->bold = bold;
}

void Text2Pdf::paragraph_set_underline(bool underline)
// Set whether text should be underlined.
{
  ensure_open_paragraph();
  input_paragraph->underline = underline;
}

void Text2Pdf::paragraph_set_small_caps(bool small_caps)
// Set whether text should be in small caps.
{
  ensure_open_paragraph();
  input_paragraph->small_caps = small_caps;
}

void Text2Pdf::paragraph_set_alignment(T2PAlignmentType alignment)
// Sets the paragraph alignment, e.g. left, or justified.
{
  ensure_open_paragraph();
  input_paragraph->alignment = alignment;
}

void Text2Pdf::paragraph_set_space_before(int millimeters)
// Sets the space before the paragraph, in millimeters.
{
  ensure_open_paragraph();
  input_paragraph->space_before_mm = millimeters;
}

void Text2Pdf::paragraph_set_space_after(int millimeters)
// Sets the desired space after the paragraph, in millimeters.
{
  ensure_open_paragraph();
  input_paragraph->space_after_mm = millimeters;
}

void Text2Pdf::paragraph_set_left_margin(int millimeters)
// Sets the paragraph's left margin, in millimeters.
{
  ensure_open_paragraph();
  input_paragraph->left_margin_mm = millimeters;
}

void Text2Pdf::paragraph_set_right_margin(int millimeters)
// // Sets the paragraph's right margin, in millimeters.
{
  ensure_open_paragraph();
  input_paragraph->right_margin_mm = millimeters;
}

void Text2Pdf::paragraph_set_first_line_indent(int millimeters)
// Sets the indent of the first line of the paragraph.
{
  ensure_open_paragraph();
  input_paragraph->first_line_indent_mm = millimeters;
}

void Text2Pdf::paragraph_set_column_count(unsigned int count)
// Sets the number of columns. Only 1 or 2 are supported.
{
  ensure_open_paragraph();
  count = CLAMP (count, 1, 2);
  input_paragraph->column_count = count;
}

void Text2Pdf::paragraph_set_keep_with_next()
// Sets that this paragraph should be kept with the next one.
{
  ensure_open_paragraph();
  input_paragraph->keep_with_next = true;
}

void Text2Pdf::close_paragraph()
// Close the open paragraph.
{
  input_paragraph = NULL;
}

void Text2Pdf::inline_set_font_size_percentage(int percentage)
// Sets the font size percentage for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_font_size_percentage(percentage);
}

void Text2Pdf::inline_clear_font_size_percentage()
// Clears the font size percentage for inline text. 
{
  inline_set_font_size_percentage(100);
}

void Text2Pdf::inline_set_italic(int italic)
// Sets the italic markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_italic((T2PMarkupType)italic);
}

void Text2Pdf::inline_clear_italic()
// Clears the italic markup for inline text. 
{
  inline_set_italic(t2pmtInherit);
}

void Text2Pdf::inline_set_bold(int bold)
// Sets the bold markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_bold((T2PMarkupType)bold);
}

void Text2Pdf::inline_clear_bold()
// Clears the bold markup for inline text. 
{
  inline_set_bold(t2pmtInherit);
}

void Text2Pdf::inline_set_underline(int underline)
// Sets the underline markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_underline((T2PMarkupType)underline);
}

void Text2Pdf::inline_clear_underline()
// Sets the underline markup for inline text. 
{
  inline_set_underline(t2pmtInherit);
}

void Text2Pdf::inline_set_small_caps(int small_caps)
// Sets the small capitals markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_small_caps((T2PMarkupType)small_caps);
}

void Text2Pdf::inline_clear_small_caps()
// Clears the small capitals markup for inline text. 
{
  inline_set_small_caps(t2pmtInherit);
}

void Text2Pdf::inline_set_superscript(bool superscript)
// Sets the superscript markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_superscript(superscript);
}

void Text2Pdf::inline_clear_superscript()
// Clears the superscript markup for inline text. 
{
  inline_set_superscript(false);
}

void Text2Pdf::inline_set_colour(int colour)
// Sets the colour for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_colour(colour);
}

void Text2Pdf::inline_clear_colour()
// Clears the colour for inline text. 
{
  inline_set_colour(0);
}

void Text2Pdf::inline_set_strike_through()
// Sets the strike-through markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  input_paragraph->inline_set_strike_through(t2pmtOn);
}

void Text2Pdf::inline_clear_strike_through()
// Clears the strike-through markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Switch the inline value off.
  input_paragraph->inline_set_strike_through(t2pmtOff);
}

void Text2Pdf::add_text(const ustring& text)
// Add text to whatever container is on top of the stack.
{
  ensure_open_paragraph();
  input_paragraph->add_text(text);
}

void Text2Pdf::open_note()
// Open a new note.
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Only open a new note if there's no note open already.
  if (stacked_input_paragraph == NULL) {
    stacked_input_paragraph = input_paragraph;
    input_paragraph = new T2PInputParagraph (font, line_spacing, right_to_left);
    // The note is stored into the main paragraph.
    stacked_input_paragraph->add_note(input_paragraph);
  }
  // By default there's no first-line indent.
  paragraph_set_first_line_indent(0);
  // By default a smaller font is used for the note.
  paragraph_set_font_size((0.8 * stacked_input_paragraph->font_size_points));
  // By default the note is left aligned.
  paragraph_set_alignment(t2patLeft);
}

void Text2Pdf::close_note()
// Close a note.
{
  if (stacked_input_paragraph) {
    input_paragraph = stacked_input_paragraph;
    stacked_input_paragraph = NULL;
  }
}

void Text2Pdf::open_intrusion()
// Open a new intrusion.
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Only open a new intrusion if there's nothing open already.
  if (stacked_input_paragraph == NULL) {
    stacked_input_paragraph = input_paragraph;
    input_paragraph = new T2PInputParagraph (font, line_spacing, right_to_left);
    // The intrusion is stored into the main paragraph.
    stacked_input_paragraph->set_intrusion(input_paragraph);
  }
  // By default there's no first-line indent.
  paragraph_set_first_line_indent(0);
  // By default the instrusion is left aligned.
  paragraph_set_alignment(t2patLeft);
}

void Text2Pdf::close_intrusion()
// Closes an intrusion.
{
  if (stacked_input_paragraph) {
    input_paragraph = stacked_input_paragraph;
    stacked_input_paragraph = NULL;
  }
}

void Text2Pdf::view()
// View the pdf file.
{
  pdfviewer_view(pdffile);
}

void Text2Pdf::set_font(const ustring& font_in)
/* 
 Sets the font to use. The font can be set multiple times. 
 Each paragraph will use the font that was current at the time that the input paragraph object was created.
 */
{
  font = font_in;
}

void Text2Pdf::set_line_spacing(unsigned int line_spacing_in)
// Sets the line spacing for subsequent paragraphs.
{
  line_spacing = line_spacing_in;
}

void Text2Pdf::set_right_to_left(bool right_to_left_in)
// Sets the right-to-left property for subsequent paragraphs.
{
  right_to_left = right_to_left_in;
}

void Text2Pdf::print_date_in_header()
// Print the date in the header.
{
  print_date = true;
}

void Text2Pdf::test() {
  // White background.
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_paint(cairo);

  // Size of the layout.
  PangoRectangle rectangle;
  rectangle.x = inside_margin_pango_units;
  rectangle.y = top_margin_pango_units;
  rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
  rectangle.height = 0;

  // New Pango layout.
  PangoLayout * layout = pango_cairo_create_layout(cairo);

  // Font.
  PangoFontDescription *desc;
  desc = pango_font_description_from_string("Cardo 12");
  pango_font_description_set_size(desc, 12 * PANGO_SCALE);
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  // Set Layout's parameters.
  PangoAttrList *attrs = pango_attr_list_new();
  pango_layout_set_width(layout, rectangle.width);
  pango_layout_set_text(layout, " Ekuqalenia", -1);
  pango_layout_set_attributes(layout, attrs);
  pango_attr_list_unref(attrs);

  // Paint the layout.  
  cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
  cairo_move_to(cairo, pango_units_to_points(rectangle.x), pango_units_to_points(rectangle.y));
  pango_cairo_show_layout(cairo, layout);

  // Write page.
  cairo_show_page(cairo);
}

/*

 Todo text2pdf 

 If the c-style is inserted, it asks whether to insert a new chapter. If no is replied, it still modifies
 the style of the current paragraph.
 
 If the notes are written in full, the formatter hangs. To fix that.
 
 To implement negative space following the paragraph by making the height of the block lower.
 
 To go through all of the Usfm2Text object and implement missing bits.
 
 To implement running headers.
 
 To implement USFM to TextInput converter. Many parts are still left unimplemented.
 
 To implement renumbering note callers per page as in code similar to:
 caller_in_text = notecallers[stylepointer]->renumber_per_page_temporal_caller_text;
 We need to have a .note_caller() function, which can just pass the caller to be formatted literally,
 or in cases that the numbering restarts per page, we can pass a pointer to a NoteCaller object.
 There can be various NoteCaller objects, one for f, fe, x, and one for the caller in the text, and one for the 
 caller in the note, as these differ.
 Once this starts, we don't know how many space the note callers will take up. If the renumbering restarts 
 every page, then in such a situation we don't know whether the space will be one or two digits.
 To cover this situation too, we need to lay out the text in the page while generating the layouts from the input stream.
 So this means that there is a continual input stream, and that the layout process is driven by that stream.
 
 To implement images rendering, probably png only as cairo reads them natively.
 When images are rendered, these go into the LayoutContainer object, though the name may have to be changed at that stage.
 
 Try right-to-left text such as Hebrew, and Farsi.

 Once we've all dependencies on the fontconfig library out, then we can remove this library, simplifying the installation procedures.
 
 The small caps attribute is not yet implemented in Pango. The typesetter should make its own implementation.
 There's some information about this in the object where the layouting is done.
 
 There's going to be an option to let the text2pdf input write its input to file, optionally,
 so that people can see what codes are used.
 Some code like:
 open_paragraph
 paragraph_set_font_size 10
 Very, very simple markup, line based.
 Then there's another option in the menu that lets the text2pdf object read its input from a file, and makes 
 pdf out of that. By doing so the user has maximum flexibility to print his own bible and make corrections 
 that bibledit can't do yet.
 
 In the new stylesheet, the "superscript" property in the stylesheet is enough to both raise the letter
 and to set the new font size, and make it slightly heavier.
 To remove the font size property altogether, so as to make things easier.
 If users wish more control, they can do the manual conversion routine.
 Footnotes don't need superscript, except for the caller in the text.
 The footnotes themselves just use the same characters for both caller in note and note text itself.
 
 When laying out long paragraphs, this takes a huge amount of time.
 Therefore we don't load the full paragraph in the PangoLayout each time, but
 a maximum of so many characters only, or load the first one fully, then measure length of characters, then
 subsequent timse only load double the measured characters.
 
 The "end-indent" may no longer be needed anywhere.
 
 Setting the Printing fonts can go out.
 
 The line height should affect the text in the Editor also.
 
 The fonts.c / cpp files go out.
 
 The xep related code goes out.
 
 Line spacing should affect printed text, also plain usfm printing.
 
 pango version 1.18 and up has the justify property implemented.
 To try out in a virtual machine whether Debian 4.0 can be upgraded to the version of Pango that has justify working.
 But then, justify may not work right because it won't justify the last line, and in our way of doing things,
 every line is the last line. So it will never justify.
 
 The right-to-left property should be properly made working, even to such a degree that the columns start
 at the right instead of at the left, and that the drop-caps chapter number is at the right too.

 Suggestion: change size of verse number to around 70% of normal font. 
 Top of verse number should be even with top of capital T. 
 Check some Bibles to compare to see if this is correct.  
 Unless you like this to help you find your way around. 
 
 The xrefs with references written in full still hangs, probably it hangs on the layout stage if something does
 not fit on the page.
 
 */

