/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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


#include "xetex2pdf.h"
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
#include "gtkwrappers.h"
#include "pdfviewer.h"
#include "settings.h"
#include "referenceutils.h"
#include "projectutils.h"
#include <math.h>
#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include "text2pdf_intermediate.h"
#include "dialogtext2pdf.h"
#include "runtime.h"


XeTeX2Pdf::XeTeX2Pdf(int dummy)
// Converts XeTeX code to a pdf file.
{
  // Initializers.
  initialize_variables();

  // Create the cairo variables.
  //surface = cairo_pdf_surface_create(pdffile.c_str(), pango_units_to_points(page_width_pango_units), pango_units_to_points(page_height_pango_units));
  //cairo = cairo_create(surface);
}


XeTeX2Pdf::~XeTeX2Pdf()
// Object destructor.
{
  /*
  for (unsigned int i = 0; i < input_data.size(); i++) {
    delete input_data[i];
  }
  for (unsigned int i = 0; i < pages.size(); i++) {
    delete pages[i];
  }
  cairo_surface_destroy(surface);
  cairo_destroy(cairo);
  */
}


void XeTeX2Pdf::initialize_variables()
// Initialize the variables for the object.
{
  // The page size defaults to A4 = 210 x 297 millimeters (8.27 x 11.69 inches).
  page_width_pango_units = millimeters_to_pango_units(210);
  page_height_pango_units = millimeters_to_pango_units(297);

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
  keep_data_together = false;
  line_spacing = 100;
  right_to_left = false;

  // Running headers.
  print_date = false;
  running_chapter_left_page = 0;
  running_chapter_right_page = 0;
  suppress_header_on_this_page = false;
}


void XeTeX2Pdf::page_size_set(double width_centimeters, double height_centimeters)
// Set the page size in centimeters.
{
  page_width_pango_units = centimeters_to_pango_units(width_centimeters);
  page_height_pango_units = centimeters_to_pango_units(height_centimeters);
  //cairo_pdf_surface_set_size(surface, pango_units_to_points(page_width_pango_units), pango_units_to_points(page_height_pango_units));
}


void XeTeX2Pdf::page_margins_set(double inside_margin_centimeters, double right_margin_centimeters, double top_margin_centimeters, double bottom_margin_centimeters)
// Set the margins of the page, in centimeters.
{
  inside_margin_pango_units = centimeters_to_pango_units(inside_margin_centimeters);
  outside_margin_pango_units = centimeters_to_pango_units(right_margin_centimeters);
  top_margin_pango_units = centimeters_to_pango_units(top_margin_centimeters);
  bottom_margin_pango_units = centimeters_to_pango_units(bottom_margin_centimeters);
}


void XeTeX2Pdf::header_height_set(double size_centimeters)
// Set the height of the header.
{
  header_height_pango_units = centimeters_to_pango_units(size_centimeters);
}


void XeTeX2Pdf::footer_height_set(double size_centimeters)
// Set the height of the footer.
{
  footer_height_pango_units = centimeters_to_pango_units(size_centimeters);
}


void XeTeX2Pdf::column_spacing_set(double spacing_centimeters)
// Sets the spacing between the two columns.
// Text is normally laid out in two columns, but there are elements that span the columns.
{
  column_spacing_pango_units = centimeters_to_pango_units(spacing_centimeters);
}


void XeTeX2Pdf::page_one_column_only()
// Sets that there's only one column on the pages.
{
  one_column_only = true;
}


void XeTeX2Pdf::new_page(bool odd)
// Sets the engine to go to a new page. If odd is true, it goes to a new odd page.
{
  // Close any open paragraph.
  close_paragraph();

  // The paragraph before the new page should not have its 'keep-with-next' property set, else the engine chokes on that.
  /*
  if (!input_data.empty()) {
    T2PInput *preceding_input = input_data[input_data.size() - 1];
    if (preceding_input->type == t2pitParagraph) {
      ((T2PInputParagraph *) preceding_input)->keep_with_next = false;
    }
  }
  */
  // Store the new page.
  /*
  if (odd)
    input_data.push_back(new T2PInput(t2pitNewOddPage));
  else
    input_data.push_back(new T2PInput(t2pitNewPage));
    */
}


void XeTeX2Pdf::run()
// Runs the converter.
{
  // Add the \bye command.
  xetex_lines.push_back ("\\bye");

  // Save the XeTeX input data to disk.
  ustring filename_tex = gw_build_filename (directories_get_temp (), "document.tex");
  write_lines (filename_tex, xetex_lines);

  // Run xetex.
  if (runtime_check (rtXeTeX)) {
    GwSpawn spawn (runtime_program (rtXeTeX));
    spawn.workingdirectory (directories_get_temp());
    spawn.arg (filename_tex);
    spawn.progress ("Typesetting", true);
    spawn.run ();
  }




/*
  // Create progress window.
  progresswindow = new ProgressWindow("Formatting", false);
  progresswindow->set_text("Run");

  // Close any open input containers.
  close_paragraph();

  // Go through the input data.
  run_input();

  // Find potential widows and orphans.
  widows_and_orphans_and_keep_with_next();

  // Fit the blocks on the pages.
  fit_blocks_on_pages();

  // Generate tables of contents.
  generate_tables_of_contents();
  */

  // If there are no pages, produce one empty page - looks better.
  /*
  if (pages.empty()) {
    next_page();
  }
  */

  // Print the pages.
  /*
  progresswindow->set_iterate(0, 1, pages.size());
  progresswindow->set_text("Output");
  for (unsigned int pg = 0; pg < pages.size(); pg++) {
    progresswindow->iterate();
    T2PPage *page = pages[pg];
    page->print();
  }

  // Status information.
  status = cairo_surface_status(surface);
  if (status != CAIRO_STATUS_SUCCESS) {
    gw_warning(cairo_status_to_string(status));
  }
  */
}


void XeTeX2Pdf::run_input()
// Goes through all of the input data.
{
/*
  progresswindow->set_iterate(0, 1, input.size());
  progresswindow->set_text("Layout");
  for (unsigned int i = 0; i < input.size(); i++) {
    progresswindow->iterate();
    switch (input[i]->type) {
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
        T2PBlock *block = new T2PBlock(rectangle, 1);
        block->type = t2pbtSpaceAfterParagraph;
        input_blocks.push_back(block);
        break;
      }
    case t2pitNewPage:
      {
        PangoRectangle rectangle;
        rectangle.x = 0;
        rectangle.y = 0;
        rectangle.width = 0;
        rectangle.height = 0;
        T2PBlock *block = new T2PBlock(rectangle, 1);
        block->type = t2pbtNewPage;
        input_blocks.push_back(block);
        break;
      }
    case t2pitNewOddPage:
      {
        PangoRectangle rectangle;
        rectangle.x = 0;
        rectangle.y = 0;
        rectangle.width = 0;
        rectangle.height = 0;
        T2PBlock *block = new T2PBlock(rectangle, 1);
        block->type = t2pbtNewOddPage;
        input_blocks.push_back(block);
        break;
      }
    }
  }
  */
}


void XeTeX2Pdf::lay_out_paragraph()
// Lays out one paragraph.
{
  /*
  // Bail out if there's no text.
  if (//input_paragraph->text.empty()) {
    return;
  }
  // Optionally lay out the paragraph's intrusion.
  T2PInputParagraph *intrusion = //input_paragraph->get_intrusion();
  if (intrusion) {
    get_next_layout_container(true);
    layoutcontainer->layout_drop_caps(intrusion, //input_paragraph->font_size_points);
  }
  // Space before paragraph.
  if (//input_paragraph->space_before_mm != 0) {
    PangoRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
    rectangle.height = millimeters_to_pango_units(//input_paragraph->space_before_mm);
    int column_count = get_column_count_rectangle_width(//input_paragraph->column_count, rectangle.width);
    T2PBlock *block = new T2PBlock(rectangle, column_count);
    block->type = t2pbtSpaceBeforeParagraph;
    block->keep_with_next = true;
    input_blocks.push_back(block);
  }
  // Paragraph itself.
  unsigned int line_number = 0;
  while (!//input_paragraph->text.empty() && line_number < 1000) {
    get_next_layout_container(false);
    layoutcontainer->layout_text(input_paragraph, line_number, //input_paragraph->text);
    line_number++;
  }
  if (!//input_paragraph->text.empty()) {
    gw_warning("Can't fit in \"" + //input_paragraph->text + "\"");
  }
  // Space after paragraph.
  if (//input_paragraph->space_after_mm != 0) {
    PangoRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
    rectangle.height = millimeters_to_pango_units(//input_paragraph->space_after_mm);
    int column_count = get_column_count_rectangle_width(//input_paragraph->column_count, rectangle.width);
    T2PBlock *block = new T2PBlock(rectangle, column_count);
    block->type = t2pbtSpaceAfterParagraph;
    block->keep_with_next = //input_paragraph->keep_with_next;
    input_blocks.push_back(block);
  }
  */
}


void XeTeX2Pdf::get_next_layout_container(bool intrusion)
// Gets the next layout container to be used.
{
/*
  // Rectangle for next block.
  PangoRectangle rectangle;
  rectangle.x = 0;
  rectangle.y = 0;
  // Only the width of the block is set, because the other sizes depend on factors not yet known at this stage.
  rectangle.width = page_width_pango_units - inside_margin_pango_units - outside_margin_pango_units;
  rectangle.height = 0;

  // Column count. If there's more than one, the width gets smaller.
  int column_count = get_column_count_rectangle_width(//input_paragraph->column_count, rectangle.width);

  // Create a new block.
  T2PBlock *block = new T2PBlock(rectangle, column_count);
  block->keep_with_next = //input_paragraph->keep_with_next || keep_data_together || intrusion;
  if (intrusion) {
    block->type = t2pbtTextIntrusion;
  }
  // Running header information.
  block->left_running_header = //input_paragraph->left_running_header;
  block->right_running_header = //input_paragraph->right_running_header;
  block->left_running_chapter = //input_paragraph->left_running_chapter;
  block->right_running_chapter = //input_paragraph->right_running_chapter;
  block->suppress_header = //input_paragraph->suppress_header;
  //input_paragraph->suppress_header = false;
  // Table of contents.
  block->reference = //input_paragraph->reference;
  input_paragraph->reference.clear();
  block->referent = input_paragraph->referent;
  input_paragraph->referent.clear();

  // Handle a preceding intrusion.
  if (!input_blocks.empty()) {
    unsigned int preceding_block_to = input_blocks.size();
    int preceding_block_from = preceding_block_to;
    // Look 2 blocks back, because by default the intrusion, that's the drop-caps, is two lines high.
    preceding_block_from -= 2;
    if (preceding_block_from < 0)
      preceding_block_from = 0;
    for (unsigned int i = preceding_block_from; i < preceding_block_to; i++) {
      if (input_blocks[i]->type == t2pbtTextIntrusion) {
        // Modify size and position of the block.
        // In case of right-to-left text, the x offset is not modified, 
        // because the intrusion is at the right side of the page.
        if (!right_to_left) {
          block->rectangle.x += input_blocks[i]->rectangle.width;
        }
        block->rectangle.width -= input_blocks[i]->rectangle.width;
        // Block stays with the next one.
        block->keep_with_next = true;
      }
    }
  }
  // Store the block.
  input_blocks.push_back(block);

  // Create a new layout container.
  layoutcontainer = block->next_layout_container(cairo);
  */
}


void XeTeX2Pdf::fit_blocks_on_pages()
// Fits the block onto the pages, and creates new pages if need be.
{
  /*
  // Progress.
  unsigned int initial_input_size = input_blocks.size();
  progresswindow->set_text("Pages");

  // Loop through the input blocks.
  unsigned int infinite_loop_counter = 0;
  while (!input_blocks.empty() && infinite_loop_counter < 5000) {

    // Create another page.
    next_page();

    // Progress.
    double fraction = double (initial_input_size - input_blocks.size()) / initial_input_size;
    progresswindow->set_fraction(fraction);

    // Lay out the input on the page.
    page->text_reference_area->fit_blocks(input_blocks, column_spacing_pango_units);

    // Check whether to start a new odd page.
    if (page->text_reference_area->start_new_odd_page) {
      // If the current page is odd, then the next will be even, so we need to insert an extra one here.
      int current_page_number = pages.size();
      if (current_page_number % 2) {
        next_page();
      }
    }
  }
  */
}


void XeTeX2Pdf::widows_and_orphans_and_keep_with_next()
/*
 Widows are the first lines of a paragraph, while the rest is in another column or on another page.
 Orphans are the last lines of a pragraph in such a situation.
 We set two widows and two orphans.
 The first two lines of a paragraph could be widows.
 The last two lines of a paragraph could be orphans.
 */
{
  /*
  // Widows and orphans control is is implemented throught the "keep_with_next" property of the T2PBlock object.
  // The "keep with next" mechanism will then take over when the blocks are fitted in.
  for (unsigned int blk = 0; blk < input_blocks.size(); blk++) {
    T2PBlock *block = input_blocks[blk];
    // The first line of the paragraph is to be kept with the next.
    if (block->type == t2pbtTextParagraphFirstLine) {
      block->keep_with_next = true;
    }
    // The last-but-one line of the paragraph is to be kept with the next.
    if (block->type == t2pbtTextParagraphLastLine) {
      if (blk) {
        input_blocks[blk - 1]->keep_with_next = true;
      }
    }
  }
  // If the "keep-with-next" property is set just before a new page, 
  // the engine chokes on that, so it is cleared here.
  for (unsigned int blk = 0; blk < input_blocks.size(); blk++) {
    T2PBlock *block = input_blocks[blk];
    // The first line of the paragraph is to be kept with the next.
    if (block->type == t2pbtNewPage || block->type == t2pbtNewOddPage) {
      if (blk > 0) {
        T2PBlock *preceding_block = input_blocks[blk - 1];
        preceding_block->keep_with_next = false;
      }
    }
  }
  */
}


int XeTeX2Pdf::get_column_count_rectangle_width(int column_count_in, int &width)
{
  int column_count = one_column_only ? 1 : column_count_in;
  if (column_count > 1) {
    width /= column_count;
    width -= (column_count - 1) * column_spacing_pango_units / 2;
  }
  return column_count;
}


void XeTeX2Pdf::next_page()
// Produces the next page and related objects.
{
  /*
  int page_number = pages.size() + 1;
  page = new T2PPage(page_number, page_width_pango_units, page_height_pango_units, inside_margin_pango_units, outside_margin_pango_units, top_margin_pango_units, bottom_margin_pango_units, header_height_pango_units, footer_height_pango_units, cairo, print_date, right_to_left);
  pages.push_back(page);
  */
}


void XeTeX2Pdf::open_keep_together()
// Anything between this and the closer is kept together in one page or column.
{
  close_paragraph();
  //input_data.push_back(new T2PInput(t2pitOpenKeepTogether));
}


void XeTeX2Pdf::close_keep_together()
// Close keeping together in one page or column.
{
  close_paragraph();
  //input_data.push_back(new T2PInput(t2pitCloseKeepTogether));
}


void XeTeX2Pdf::open_paragraph()
// Open a new paragraph and add this to the input data.
{
  // Close existing one.
  close_paragraph();
  // Create new one.
  //input_paragraph = new T2PInputParagraph(font, line_spacing, right_to_left);
  // Store running header information.
  //input_paragraph->left_running_header = running_header_left_page;
  //input_paragraph->right_running_header = running_header_right_page;
  //input_paragraph->left_running_chapter = running_chapter_left_page;
  //input_paragraph->right_running_chapter = running_chapter_right_page;
  // Store paragraph.
  //input_data.push_back(input_paragraph);
}


void XeTeX2Pdf::ensure_open_paragraph()
// Ensures that a paragraph is open.
{
  /*
  if (input_paragraph == NULL)
    open_paragraph();
    */
}


void XeTeX2Pdf::paragraph_set_font_size(double points)
// Sets the font size to be used in the paragraph, in points.
{
  ensure_open_paragraph();
  //input_paragraph->font_size_points = points;
}


void XeTeX2Pdf::paragraph_set_italic(bool italic)
// Set whether font should be italic.
{
  ensure_open_paragraph();
  //input_paragraph->italic = italic;
}


void XeTeX2Pdf::paragraph_set_bold(bool bold)
// Set whether font should be in bold.
{
  ensure_open_paragraph();
  //input_paragraph->bold = bold;
}


void XeTeX2Pdf::paragraph_set_underline(bool underline)
// Set whether text should be underlined.
{
  ensure_open_paragraph();
  //input_paragraph->underline = underline;
}


void XeTeX2Pdf::paragraph_set_small_caps(bool small_caps)
// Set whether text should be in small caps.
{
  ensure_open_paragraph();
  //input_paragraph->small_caps = small_caps;
}


void XeTeX2Pdf::paragraph_set_alignment(T2PAlignmentType alignment)
// Sets the paragraph alignment, e.g. left, or justified.
{
  ensure_open_paragraph();
  //input_paragraph->alignment = alignment;
}


void XeTeX2Pdf::paragraph_set_space_before(int millimeters)
// Sets the space before the paragraph, in millimeters.
{
  ensure_open_paragraph();
  //input_paragraph->space_before_mm = millimeters;
}


void XeTeX2Pdf::paragraph_set_space_after(int millimeters)
// Sets the desired space after the paragraph, in millimeters.
{
  ensure_open_paragraph();
  //input_paragraph->space_after_mm = millimeters;
}


void XeTeX2Pdf::paragraph_set_left_margin(int millimeters)
// Sets the paragraph's left margin, in millimeters.
{
  ensure_open_paragraph();
  //input_paragraph->left_margin_mm = millimeters;
}


void XeTeX2Pdf::paragraph_set_right_margin(int millimeters)
// Sets the paragraph's right margin, in millimeters.
{
  ensure_open_paragraph();
  ////input_paragraph->right_margin_mm = millimeters;
}


void XeTeX2Pdf::paragraph_set_first_line_indent(int millimeters)
// Sets the indent of the first line of the paragraph.
{
  ensure_open_paragraph();
  ////input_paragraph->first_line_indent_mm = millimeters;
}


void XeTeX2Pdf::paragraph_set_column_count(unsigned int count)
// Sets the number of columns. Only 1 or 2 are supported.
{
  ensure_open_paragraph();
  count = CLAMP(count, 1, 2);
  //input_paragraph->column_count = count;
}


void XeTeX2Pdf::paragraph_set_keep_with_next()
// Sets that this paragraph should be kept with the next one.
{
  ensure_open_paragraph();
  //input_paragraph->keep_with_next = true;
}


void XeTeX2Pdf::close_paragraph()
// Close the open paragraph.
{
  //input_paragraph = NULL;
}


void XeTeX2Pdf::inline_set_font_size_percentage(int percentage)
// Sets the font size percentage for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_font_size_percentage(percentage);
}


void XeTeX2Pdf::inline_clear_font_size_percentage()
// Clears the font size percentage for inline text. 
{
  inline_set_font_size_percentage(100);
}


void XeTeX2Pdf::inline_set_italic(int italic)
// Sets the italic markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_italic((T2PMarkupType) italic);
}


void XeTeX2Pdf::inline_clear_italic()
// Clears the italic markup for inline text. 
{
  inline_set_italic(t2pmtInherit);
}


void XeTeX2Pdf::inline_set_bold(int bold)
// Sets the bold markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_bold((T2PMarkupType) bold);
}


void XeTeX2Pdf::inline_clear_bold()
// Clears the bold markup for inline text. 
{
  inline_set_bold(t2pmtInherit);
}


void XeTeX2Pdf::inline_set_underline(int underline)
// Sets the underline markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_underline((T2PMarkupType) underline);
}


void XeTeX2Pdf::inline_clear_underline()
// Sets the underline markup for inline text. 
{
  inline_set_underline(t2pmtInherit);
}


void XeTeX2Pdf::inline_set_small_caps(int small_caps)
// Sets the small capitals markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_small_caps((T2PMarkupType) small_caps);
}


void XeTeX2Pdf::inline_clear_small_caps()
// Clears the small capitals markup for inline text. 
{
  inline_set_small_caps(t2pmtInherit);
}


void XeTeX2Pdf::inline_set_superscript(bool superscript)
// Sets the superscript markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_superscript(superscript);
}


void XeTeX2Pdf::inline_clear_superscript()
// Clears the superscript markup for inline text. 
{
  inline_set_superscript(false);
}


void XeTeX2Pdf::inline_set_colour(int colour)
// Sets the colour for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_colour(colour);
}


void XeTeX2Pdf::inline_clear_colour()
// Clears the colour for inline text. 
{
  inline_set_colour(0);
}


void XeTeX2Pdf::inline_set_strike_through()
// Sets the strike-through markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Store the inline value.
  //input_paragraph->inline_set_strike_through(t2pmtOn);
}


void XeTeX2Pdf::inline_clear_strike_through()
// Clears the strike-through markup for inline text. 
{
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Switch the inline value off.
  //input_paragraph->inline_set_strike_through(t2pmtOff);
}


void XeTeX2Pdf::add_text(ustring text)
// Add text to whatever container is on top of the stack.
{
  /*
  Escaping special characters: 
  * The usual alpha-numerical characters and punctuation marks can be used as is and behave as expected. 
  * However, some symbols have a special meaning to TeX, and to use these characters, you have to (usually) "escape" them 
  * by putting a backslash in front of the character. 
  * For example, "$10" should be typed as "\$10", and 10% as 10\%. 
  * (Note: TeX ignores blanks following an escaped character; if you want a blank (as in "10% of ..."), 
  * you have to "escape" the blank as well ("10\%\ of ...").
  Braces and parentheses: 
  * Brackets and round parentheses can be used as is and shouldn't be escaped, 
  * but curly braces ("{","}") are used for grouping in TeX, and don't get printed. 
  * To get curly braces in the output, you must use the escaped versions, "\{", and "\}".
  Quotation marks: 
  * The usual double quotation marks don't come out correctly in TeX. 
  * To get correct double quotes, surround the quoted phrase by a left and a right pair of single quotes (``TeX'').   
  */  
  //replace_text (text, "$", "\\$");
  //replace_text (text, "$ ", "$\\ ");
  //replace_text (text, "%", "\\%");
  //replace_text (text, "% ", "%\\ ");
  //replace_text (text, "{", "\\{");
  //replace_text (text, "{ ", "{\\ ");
  //replace_text (text, "{", "@{");
  //replace_text (text, "{ ", "{\\ ");
  //replace_text (text, "}", "@}");
  //replace_text (text, "} ", "}\\ ");
  //replace_text (text, "\"", "\\\"");
  //replace_text (text, "\" ", "\"\\ ");
  
  // working here, reading gentle.pdf page 10. But later the ptx2pdf macros were used instead.
  
  /* Try this:

    \documentclass{minimal}

    \usepackage{fontspec}
    \usepackage{xunicode}
    \usepackage{xltxtra}

    \begin{document}

    æ ö

    \end{document}

  
  */
  
  // Store the text. this is not correct, it should be stored not as a line, else it gets spaces where no spaces are intended.
  xetex_lines.push_back (text);
  // Add the text to the paragraph.
  ensure_open_paragraph();
  //input_paragraph->add_text(text);
  // Header suppression is done here, because only input paragraphs that have text are considered in the suppression system.
  if (suppress_header_on_this_page) {
    //input_paragraph->suppress_header = true;
  }
  suppress_header_on_this_page = false;
  // References and referents are stored here, because only input paragraphs with text are considered in the system.
  set_reference(reference);
  set_referent(referent);
}


void XeTeX2Pdf::open_note()
// Open a new note.
{
  /*
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Only open a new note if there's no note open already.
  if (stacked_input_paragraph == NULL) {
    stacked_input_paragraph = input_paragraph;
    input_paragraph = new T2PInputParagraph(font, line_spacing, right_to_left);
    // The note is stored into the main paragraph.
    stacked_input_paragraph->add_note(input_paragraph);
  }
  // By default there's no first-line indent.
  paragraph_set_first_line_indent(0);
  // By default a smaller font is used for the note.
  paragraph_set_font_size((0.8 * stacked_input_paragraph->font_size_points));
  // By default the note is left aligned.
  paragraph_set_alignment(t2patLeft);
  */
}


void XeTeX2Pdf::close_note()
// Close a note.
{
  /*
  if (stacked_input_paragraph) {
    input_paragraph = stacked_input_paragraph;
    stacked_input_paragraph = NULL;
  }
  */
}


void XeTeX2Pdf::open_intrusion()
// Open a new intrusion.
{
  /*
  // Ensure that a paragraph is open.
  ensure_open_paragraph();
  // Only open a new intrusion if there's nothing open already.
  if (stacked_input_paragraph == NULL) {
    stacked_input_paragraph = input_paragraph;
    input_paragraph = new T2PInputParagraph(font, line_spacing, right_to_left);
    // The intrusion is stored into the main paragraph.
    stacked_input_paragraph->set_intrusion(input_paragraph);
  }
  // By default there's no first-line indent.
  paragraph_set_first_line_indent(0);
  // By default the instrusion is left aligned.
  paragraph_set_alignment(t2patLeft);
  */
}


void XeTeX2Pdf::close_intrusion()
// Closes an intrusion.
{
  /*
  if (stacked_input_paragraph) {
    input_paragraph = stacked_input_paragraph;
    stacked_input_paragraph = NULL;
  }
  */
}


void XeTeX2Pdf::view()
// View the pdf file.
{
  pdfviewer_view(gw_build_filename (directories_get_temp(), "document.pdf"));
}


void XeTeX2Pdf::set_font(const ustring & font_in)
/* 
 Sets the font to use. The font can be set multiple times. 
 Each paragraph will use the font that was current at the time that the input paragraph object was created.
 */
{
  font = font_in;
}


void XeTeX2Pdf::set_line_spacing(unsigned int line_spacing_in)
// Sets the line spacing for subsequent paragraphs.
{
  line_spacing = line_spacing_in;
}


void XeTeX2Pdf::set_right_to_left(bool right_to_left_in)
// Sets the right-to-left property for subsequent paragraphs.
{
  right_to_left = right_to_left_in;
}


void XeTeX2Pdf::print_date_in_header()
// Print the date in the header.
{
  print_date = true;
}


void XeTeX2Pdf::set_running_header_left_page(const ustring & header)
// Sets the running header for the left page.
{
  running_header_left_page = header;
}


void XeTeX2Pdf::set_running_header_right_page(const ustring & header)
// Sets the running header for the left page.
{
  running_header_right_page = header;
}


void XeTeX2Pdf::set_running_chapter_number(unsigned int left, unsigned int right)
// Sets the chapter number for in the running header.
{
  running_chapter_left_page = left;
  running_chapter_right_page = right;
}


void XeTeX2Pdf::suppress_header_this_page()
// Supresses printing the headers on this page.
{
  suppress_header_on_this_page = true;
}


void XeTeX2Pdf::set_reference(const ustring & label)
// Sets a reference.
// Used for creating a table of contents with page numbers.
{
  /*
  // Store the reference.
  reference = label;
  // Bail out if there's no reference.
  if (reference.empty())
    return;
  // Bail out if there's another paragraph, because the reference is only dealt with if it is found in the main paragraph.
  if (stacked_input_paragraph)
    return;
  // If there's a paragraph with text, set the reference in it.
  // References are only dealt with if there's text in the paragraph.
  if (input_paragraph && !input_paragraph->text.empty()) {
    input_paragraph->reference = reference;
    reference.clear();
  }
  */
}


void XeTeX2Pdf::set_referent(const ustring & label)
// Sets a referent. 
// Used for creating a table of contents with page numbers.
{
  /*
  // Store the referent.
  referent = label;
  // Bail out if there's no referent.
  if (referent.empty())
    return;
  // Bail out if there's another paragraph, because the referent is only dealt with if it is found in the main paragraph.
  if (stacked_input_paragraph)
    return;
  // If there's a paragraph with text, set the referent in it.
  // Referents are only dealt with if there's text in the paragraph.
  if (input_paragraph && !input_paragraph->text.empty()) {
    input_paragraph->referent = referent;
    referent.clear();
  }
  */
}

void XeTeX2Pdf::generate_tables_of_contents()
// Generates the tables of contents.
{
  /*
  // Get referents and their page numbers.
  map < ustring, ustring > referents;
  for (unsigned int pg = 0; pg < pages.size(); pg++) {
    T2PReferenceArea *reference_area = pages[pg]->text_reference_area;
    reference_area->get_referents(referents, pg + 1);
  }
  // Write the page numbers of the referents.
  for (unsigned int pg = 0; pg < pages.size(); pg++) {
    T2PReferenceArea *reference_area = pages[pg]->text_reference_area;
    reference_area->match_and_expand_references_and_referents(referents);
  }
  */
}


void XeTeX2Pdf::display_picture(const ustring & filename)
// Display a picture.
{
  /*
     cairo_surface_t *surface;

     ustring pdffile = gw_build_filename(directories_get_temp(), "pdf.pdf");
     surface = cairo_pdf_surface_create(pdffile.c_str(), 504, 648);

     cairo_t *cairo;
     cairo = cairo_create(surface);

     cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
     cairo_paint(cairo);

     PangoLayout * layout = pango_cairo_create_layout(cairo);
     pango_layout_set_text(layout, "Ekuqaleni", -1);

     cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
     cairo_move_to(cairo, 100, 100);
     pango_cairo_show_layout(cairo, layout);

     cairo_surface_t *image;
     image = cairo_image_surface_create_from_png("../pix/biblesociety.png");

     int w = cairo_image_surface_get_width (image);
     int h = cairo_image_surface_get_height (image);

     //cairo_translate (cairo, 128.0, 128.0);
     //cairo_scale  (cairo, 2.1, 2.1);
     //cairo_translate (cairo, -128.0, -128.0);
     //cairo_translate (cairo, -0.5*w, -0.5*h);

     cairo_set_source_surface(cairo, image, 100, 120);
     cairo_paint(cairo);

     cairo_show_page(cairo);

     cairo_surface_destroy(surface);
     cairo_destroy(cairo);

     pdfviewer_view(pdffile);

   */
}
