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

#include "text2pdf_utils.h"
#include "text2pdf_ref_area.h"
#include "gwrappers.h"
#include "tiny_utilities.h"
#include "date_time_utils.h"
#include "settings.h"

T2PReferenceArea::T2PReferenceArea(PangoRectangle rectangle_in, cairo_t * cairo_in, bool right_to_left_in):
T2PArea(rectangle_in)
// This is a reference area, e.g. a header, footer, or text area.
{
  // Initialize the y value to start stacking blocks at.
  start_stacking_y = 0;
  column_spacing_pango_units = 0;
  // Cairo.
  cairo = cairo_in;
  // New page.
  start_new_page = false;
  start_new_odd_page = false;
  // Right to left.
  right_to_left = right_to_left_in;
}

T2PReferenceArea::~T2PReferenceArea()
// Destructor.
{
  for (unsigned int i = 0; i < body_blocks.size(); i++) {
    delete body_blocks[i];
  }
  for (unsigned int i = 0; i < note_layout_containers.size(); i++) {
    delete note_layout_containers[i];
  }
  for (unsigned int i = 0; i < table_of_contents_layout_containers.size(); i++) {
    delete table_of_contents_layout_containers[i];
  }
}

void T2PReferenceArea::print()
// Print the data.
{
  // Go through each block.
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    T2PBlock *block = body_blocks[blk];
    // While laying out text, the block's x and y values are relative to the parent's values.
    // For printing this needs to be adjusted.
    block->rectangle.x += rectangle.x;
    block->rectangle.y += rectangle.y;
    // Print this block.
    block->print(cairo);
  }

  // Print the notes. While doing that reposition them to the bottom of the page.
  int notes_height = get_note_height();
  for (unsigned int i = 0; i < note_layout_containers.size(); i++) {
    T2PLayoutContainer *layout_container = note_layout_containers[i];
    layout_container->rectangle.x += rectangle.x;
    layout_container->rectangle.y += rectangle.y + rectangle.height - notes_height;
    layout_container->print(cairo);
  }

  // Print table of contents data.
  for (unsigned int i = 0; i < table_of_contents_layout_containers.size(); i++) {
    T2PLayoutContainer *layout_container = table_of_contents_layout_containers[i];
    layout_container->rectangle.x += rectangle.x;
    layout_container->rectangle.y += rectangle.y;
    layout_container->print(cairo);
  }
}

void T2PReferenceArea::print(unsigned int page_number, bool print_date, const ustring & left_running_header, const ustring & right_running_header, bool suppress_header, unsigned int left_first_chapter, unsigned int left_last_chapter, unsigned int right_first_chapter, unsigned int right_last_chapter)
// Print the page number, the date, the running header.
{
  // Headers can have their fontsize set. Create a paragraph for that.
  T2PInputParagraph header_paragraph("", 100, false);
  extern Settings *settings;
  header_paragraph.font_size_points = settings->genconfig.header_font_size_get();
  header_paragraph.first_line_indent_mm = 0;

  // Page number.
  T2PLayoutContainer page_number_layout_container(rectangle, NULL, cairo);
  string pn(convert_to_string(page_number));
  if (suppress_header)
    pn.clear();
  page_number_layout_container.layout_text(&header_paragraph, 0, pn);
  if (!(page_number % 2)) {
    // Even.
    page_number_layout_container.rectangle.x += rectangle.width - page_number_layout_container.rectangle.width;
  }
  page_number_layout_container.print(cairo);

  // Date.
  T2PLayoutContainer date_layout_container(rectangle, NULL, cairo);
  if (print_date) {
    string s(date_time_julian_human_readable(date_time_julian_day_get_current(), false));
    date_layout_container.layout_text(&header_paragraph, 0, s);
  }
  if ((page_number % 2)) {
    // Odd.
    date_layout_container.rectangle.x += page_number_layout_container.rectangle.width + millimeters_to_pango_units(5);
  } else {
    // Even.
    date_layout_container.rectangle.x += rectangle.width - date_layout_container.rectangle.width - millimeters_to_pango_units(5) - page_number_layout_container.rectangle.width;
  }
  date_layout_container.print(cairo);

  // Running header.
  T2PLayoutContainer running_header_layout_container(rectangle, NULL, cairo);
  ustring running_header;
  unsigned int first_chapter, last_chapter;
  if ((page_number % 2)) {
    // Odd page.
    running_header = right_running_header;
    first_chapter = right_first_chapter;
    last_chapter = right_last_chapter;
  } else {
    // Even page.
    running_header = left_running_header;
    first_chapter = left_first_chapter;
    last_chapter = left_last_chapter;
  }
  string header(produce_running_header(running_header, suppress_header, first_chapter, last_chapter));
  if (!header.empty()) {
    running_header_layout_container.layout_text(&header_paragraph, 0, header);
    if ((page_number % 2)) {
      // Odd page.
      running_header_layout_container.rectangle.x += rectangle.width - running_header_layout_container.rectangle.width;
    } else {
      // Even page.
    }
    running_header_layout_container.print(cairo);
  }
}

void T2PReferenceArea::fit_blocks(deque < T2PBlock * >&input_blocks, int column_spacing_pango_units_in)
// Fits the blocks into the reference area.
{
  // Page full flag.
  bool page_is_full = false;
  // Store column spacing.
  column_spacing_pango_units = column_spacing_pango_units_in;
  // Deal with the blocks after grouping them by equal column count.
  deque < T2PBlock * >blocks_with_equal_column_count;
  int n_columns = 0;
  while (!input_blocks.empty()) {
    // Handle a new page too.
    if (input_blocks[0]->column_count != n_columns || new_page_input_block_encountered(input_blocks, false)) {
      fit_column(blocks_with_equal_column_count);
      // If there are still blocks left, that means that these didn't fit on the page. 
      if (!blocks_with_equal_column_count.empty()) {
        page_is_full = true;
        break;
      }
      // New page.
      if (new_page_input_block_encountered(input_blocks, true))
        break;
      n_columns = input_blocks[0]->column_count;
    }
    blocks_with_equal_column_count.push_back(input_blocks[0]);
    input_blocks.erase(input_blocks.begin());
  }
  if (!page_is_full) {
    fit_column(blocks_with_equal_column_count);
  }
  // Return any unfitted remaining blocks to the input blocks.
  for (int i = blocks_with_equal_column_count.size() - 1; i >= 0; i--) {
    input_blocks.push_front(blocks_with_equal_column_count[i]);
  }

  // Look for any last blocks on the page that have their "keep_with_next" property set.
  // If these are there, that means these should be removed from here, and be put back into 
  // the input stream, so that they can be kept with paragraphs on the next page.
  // But this only should be done if there's indeed goint to be a next page. 
  // If there's no next paragraph to be kept with, then the last block is just left where it is.
  while (!body_blocks.empty() && body_blocks[body_blocks.size() - 1]->keep_with_next && !input_blocks.empty()) {
    input_blocks.push_front(body_blocks[body_blocks.size() - 1]);
    body_blocks.pop_back();
  }

  // If a new page block was found, remove it from memory.
  if (new_page_input_block_encountered(input_blocks, true)) {
    if (!input_blocks.empty()) {
      delete input_blocks[0];
      input_blocks.pop_front();
    }
  }
}

void T2PReferenceArea::fit_column(deque < T2PBlock * >&input_blocks)
// Tries to fit in a group of blocks that has the same column count.
{
  // Bail out if there's no input.
  if (input_blocks.empty())
    return;

  // Bail out if the reference area is already fully stacked.
  if (start_stacking_y >= rectangle.height)
    return;

  // Bail out if the next input block starts a new page.
  if (new_page_input_block_encountered(input_blocks, true)) {
    return;
  }
  // Fit the column(s) in.
  fit_columns(input_blocks, input_blocks[0]->column_count);
}

void T2PReferenceArea::fit_columns(deque < T2PBlock * >&input_blocks, int column_count)
/*
 Fits the input blocks into one or two columns.

 Fitting one column is easy: Just stack the blocks till the area is full.
 
 Fitting two columns requires a little more effort.
 If there are any input blocks available for fitting in, then instead of considering the 
 blocks one by one, it may happen that several blocks are considered at once.
 This happens if the T2PBlock->keep_with_next property is set.
 It tries to fit the next group of blocks into the second column.
 Then it keeps shifting blocks from the first to the second column, and vice-versa,
 till the smallest height for both columns has been reached.
 
 Space after a paragraph at the end of the reference area should be dismissed, 
 but it is not implemented because that situation rarely happens.
 Space before a paragraph is dismissed at the start of the reference area,
 and at the end of each column.
 
 Points to test: Whether one and two columns are fitted properly.
 Whether white space before the paragraph disappears at the top of the reference area.
 Whether the balancing works fine.
 
 */
{
  // Container for the columns.
  deque < T2PBigBlock > first_column;
  deque < T2PBigBlock > last_column;

  // Go through the input blocks.
  while (!input_blocks.empty()) {

    // Get the next lot of blocks that stay together, fit it, and remove it from the input stream.
    T2PBigBlock big_block = get_next_big_block_to_be_kept_together(input_blocks, column_count);
    last_column.push_back(big_block);
    for (unsigned int i = 0; i < big_block.blocks.size(); i++) {
      input_blocks.pop_front();
    }

    // Create any notes caused by the big block.
    deque < T2PLayoutContainer * >added_notes;
    for (unsigned int i = 0; i < big_block.blocks.size(); i++) {
      T2PBlock *block = big_block.blocks[i];
      for (unsigned int i2 = 0; i2 < block->layoutcontainers.size(); i2++) {
        T2PLayoutContainer *layout_container = block->layoutcontainers[i2];
        for (unsigned int i3 = 0; i3 < layout_container->note_paragraphs.size(); i3++) {
          T2PInputParagraph *note_paragraph = layout_container->note_paragraphs[i3];
          while (!note_paragraph->text.empty()) {
            PangoRectangle note_rectangle = get_next_free_note_rectangle();
            T2PLayoutContainer *note_layout_container = new T2PLayoutContainer(note_rectangle, NULL, cairo);
            note_layout_container->set_has_note();
            note_layout_container->layout_text(note_paragraph, 0, note_paragraph->text);
            note_layout_containers.push_back(note_layout_container);
            added_notes.push_back(note_layout_container);
          }
        }
      }
    }

    // Balance columns if there's more than one column.
    if (column_count > 1) {
      balance_columns(first_column, last_column);
    }
    // See if the column(s) didn't get too high after adding this last big block.
    int first_column_height = get_column_height(first_column, start_stacking_y);
    int last_column_height = get_column_height(last_column, start_stacking_y);
    bool columns_too_high = (start_stacking_y + MAX(first_column_height, last_column_height)) > (rectangle.height - get_note_height());
    if (columns_too_high) {

      // If there's only one block on the page, special handling is needed to avoid an infinite loop.
      bool one_block_on_page = first_column.size() + last_column.size() == 1;
      if (!one_block_on_page) {

        // Remove the last big block and balance the columns again.
        if (!last_column.empty()) {
          last_column.pop_back();
        } else {
          first_column.pop_back();
        }
        if (column_count > 1) {
          balance_columns(first_column, last_column);
        }
        // Put the blocks back to the input stream.
        for (int i = big_block.blocks.size() - 1; i >= 0; i--) {
          input_blocks.push_front(big_block.blocks[i]);
        }

      }
      // If the last big block caused any notes to be added, normally put the text of these notes back into their 
      // input paragraph, remove the notes, and destroy them.
      while (!added_notes.empty()) {
        T2PLayoutContainer *note_layout_container = note_layout_containers[note_layout_containers.size() - 1];
        if (!one_block_on_page)
          note_layout_container->undo_layout_text();
        delete note_layout_container;
        note_layout_containers.pop_back();
        added_notes.pop_back();
        // If there's only one block on the page, we have special treatment so that not all note data is removed,
        // but only excess data. This causes that bits of notes are not printed.
        if (one_block_on_page) {
          columns_too_high = (start_stacking_y + MAX(first_column_height, last_column_height)) > (rectangle.height - get_note_height());
          if (!columns_too_high)
            break;
        }
      }

      // Bail out.
      break;
    }

  }

  // Set the position of each individual block.
  // When text runs from right to left, the columns get swapped, the intrusions flush to the right,
  // The last lines of the paragraphs are flushed to the right.
  // Copy the blocks from the columns into the object.
  int second_x = rectangle.width - ((rectangle.width - column_spacing_pango_units) / 2);
  int first_column_y = start_stacking_y;
  for (unsigned int i = 0; i < first_column.size(); i++) {
    int column_x = 0;
    if (first_column[i].column_count > 1)
      if (right_to_left) {
        column_x = second_x;
      }
    first_column[i].set_blocks_x(column_x);
    int column_height = first_column[i].height(first_column_y);
    first_column[i].set_blocks_y(first_column_y);
    first_column_y += column_height;
    for (unsigned int i2 = 0; i2 < first_column[i].blocks.size(); i2++) {
      T2PBlock * block = first_column[i].blocks[i2];
      // Store the block.
      body_blocks.push_back(block);
    }
  }
  int last_column_y = start_stacking_y;
  for (unsigned int i = 0; i < last_column.size(); i++) {
    int column_x = second_x;
    if ((last_column[i].column_count == 1) || right_to_left) 
      column_x = 0;
    last_column[i].set_blocks_x(column_x);
    int column_height = last_column[i].height(last_column_y);
    last_column[i].set_blocks_y(last_column_y);
    last_column_y += column_height;
    for (unsigned int i2 = 0; i2 < last_column[i].blocks.size(); i2++) {
      T2PBlock * block = last_column[i].blocks[i2];
      // Store the block.
      body_blocks.push_back(block);
    }
  }

  // Store maximum height of any column as the starting point for the next column.
  start_stacking_y = MAX(first_column_y, last_column_y);
}

T2PBigBlock T2PReferenceArea::get_next_big_block_to_be_kept_together(deque < T2PBlock * >&input_blocks, int column_count)
// Looks at "input_blocks", and gets the next blocks to be kept together.
// It does that by considering the "keep_with_next" property of T2PBlock.
// The big blocks returned could contain one block in the normal case, and more if these are kept
// together.
// It allocates the object, and the caller should free it.
{
  T2PBigBlock big_block(column_count);
  for (unsigned int i = 0; i < input_blocks.size(); i++) {
    if (new_page_input_block_encountered(input_blocks, true)) {
      break;
    }
    big_block.blocks.push_back(input_blocks[i]);
    if (!input_blocks[i]->keep_with_next)
      break;
  }
  big_block.calculate_height();
  return big_block;
}

int T2PReferenceArea::get_column_height(deque < T2PBigBlock > &column, int reference_y)
// Gets the height of the column.
{
  int height = 0;
  for (unsigned int i = 0; i < column.size(); i++) {
    height += column[i].height(reference_y + i);
  }
  return height;
}

PangoRectangle T2PReferenceArea::get_next_free_note_rectangle()
// Gets the next free rectangle where a note could be laid out.
{
  // Calculate the next x and y.
  int next_x = 0;
  int next_y = 0;
  int last_height = 0;
  if (!note_layout_containers.empty()) {
    T2PLayoutContainer *last_container = note_layout_containers[note_layout_containers.size() - 1];
    next_x = last_container->rectangle.x + last_container->rectangle.width;
    next_x += millimeters_to_pango_units(3);
    next_y = last_container->rectangle.y;
    last_height = last_container->rectangle.height;
  }
  // If there's not enough space for the note on the x-axis, move it to the next line.
  double remaining_millimeters = pango_units_to_millimeters(rectangle.width - next_x);
  if (remaining_millimeters < 20) {
    next_x = 0;
    next_y += last_height;
  }
  // Create next rectangle. 
  PangoRectangle next_rectangle;
  next_rectangle.x = next_x;
  next_rectangle.y = next_y;
  next_rectangle.width = rectangle.width - next_x;
  next_rectangle.height = 0;
  return next_rectangle;
}

int T2PReferenceArea::get_note_height()
// Gets the height of the currently fitted notes.
{
  int height = 0;
  if (!note_layout_containers.empty()) {
    T2PLayoutContainer *last_container = note_layout_containers[note_layout_containers.size() - 1];
    height += last_container->rectangle.y;
    height += last_container->rectangle.height;
  }
  return height;
}

void T2PReferenceArea::balance_columns(deque < T2PBigBlock > &first_column, deque < T2PBigBlock > &last_column)
// Two-way balancing of the first and the second column.
// Two-way balancing means that content can be moved from the first column to the last one, 
// or from the last one to the first, depending on the needs.
{
  int last_higher_total_height = balance_last_column_higher_than_or_equal_to_first_column(first_column, last_column);
  int first_higher_total_height = balance_first_column_higher_than_or_equal_to_last_column(first_column, last_column);
  if (first_higher_total_height > last_higher_total_height) {
    balance_last_column_higher_than_or_equal_to_first_column(first_column, last_column);
  }
}

int T2PReferenceArea::balance_last_column_higher_than_or_equal_to_first_column(deque < T2PBigBlock > &first_column, deque < T2PBigBlock > &last_column)
// Move block from the first column to the last till the last column is higher than or equal to the first one.
// Returns the maximum height of both columns.
{
  int first_column_height = get_column_height(first_column, start_stacking_y);
  int last_column_height = get_column_height(last_column, start_stacking_y);
  while (last_column_height <= first_column_height && !first_column.empty()) {
    T2PBigBlock last_big_block_of_first_column = first_column[first_column.size() - 1];
    last_column.push_front(last_big_block_of_first_column);
    first_column.pop_back();
    first_column_height = get_column_height(first_column, start_stacking_y);
    last_column_height = get_column_height(last_column, start_stacking_y);
  }
  return MAX(first_column_height, last_column_height);
}

int T2PReferenceArea::balance_first_column_higher_than_or_equal_to_last_column(deque < T2PBigBlock > &first_column, deque < T2PBigBlock > &last_column)
// Move blocks from the last column to the first till the first column is higher than or equal to the last one.
// Returns the maximum height of both columns.
{
  int first_column_height = get_column_height(first_column, start_stacking_y);
  int last_column_height = get_column_height(last_column, start_stacking_y);
  while (last_column_height >= first_column_height && !last_column.empty()) {
    T2PBigBlock first_big_block_of_second_column = last_column[0];
    first_column.push_back(first_big_block_of_second_column);
    last_column.pop_front();
    first_column_height = get_column_height(first_column, start_stacking_y);
    last_column_height = get_column_height(last_column, start_stacking_y);
  }
  return MAX(first_column_height, last_column_height);
}

bool T2PReferenceArea::has_content()
// Returns whether there's any text content on this page.
{
  return !body_blocks.empty();
}

ustring T2PReferenceArea::left_running_header()
// Gets the left running header.
{
  ustring header;
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    T2PBlock *block = body_blocks[blk];
    if (!block->left_running_header.empty())
      header = block->left_running_header;
  }
  return header;
}

ustring T2PReferenceArea::right_running_header()
// Gets the right running header.
{
  ustring header;
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    T2PBlock *block = body_blocks[blk];
    if (!block->right_running_header.empty())
      header = block->right_running_header;
  }
  return header;
}

unsigned int T2PReferenceArea::left_running_first_chapter()
// Gets the running first left chapter. Zeroes don't count.
{
  unsigned int first_chapter = 0;
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    if (first_chapter == 0) {
      T2PBlock *block = body_blocks[blk];
      first_chapter = block->left_running_chapter;
    }
  }
  return first_chapter;
}

unsigned int T2PReferenceArea::right_running_first_chapter()
// Gets the running first right chapter. Zeroes don't count.
{
  unsigned int first_chapter = 0;
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    if (first_chapter == 0) {
      T2PBlock *block = body_blocks[blk];
      first_chapter = block->right_running_chapter;
    }
  }
  return first_chapter;
}

unsigned int T2PReferenceArea::left_running_last_chapter()
// Gets the running last left chapter. Zeroes don't count.
{
  unsigned int last_chapter = 0;
  for (int blk = body_blocks.size() - 1; blk >= 0; blk--) {
    if (last_chapter == 0) {
      T2PBlock *block = body_blocks[blk];
      last_chapter = block->left_running_chapter;
    }
  }
  return last_chapter;
}

unsigned int T2PReferenceArea::right_running_last_chapter()
// Gets the running last right chapter. Zeroes don't count.
{
  unsigned int last_chapter = 0;
  for (int blk = body_blocks.size() - 1; blk >= 0; blk--) {
    if (last_chapter == 0) {
      T2PBlock *block = body_blocks[blk];
      last_chapter = block->right_running_chapter;
    }
  }
  return last_chapter;
}

ustring T2PReferenceArea::produce_running_header(const ustring & header, bool suppress, unsigned int first_chapter, unsigned int last_chapter)
// This produces the running header.
{
  // The running header.
  ustring running_header(header);

  // Chapter part.
  ustring chapter_part;

  // Only put chapter part if there's something to be put.
  if (first_chapter || last_chapter) {

    // Put first chapter.
    if (first_chapter == 0)
      first_chapter++;
    chapter_part.append(convert_to_string(first_chapter));

    // If last one differs, put that too.
    if (first_chapter != last_chapter) {
      string joint;
      if (last_chapter == (first_chapter + 1))
        joint = ",";
      else
        joint = "-";
      chapter_part.append(joint);
      chapter_part.append(convert_to_string(last_chapter));
    }

  }
  // Add chapter part.
  if (!running_header.empty() && !chapter_part.empty())
    running_header.append(" ");
  running_header.append(chapter_part);

  // Suppress?
  if (suppress)
    running_header.clear();

  // Result.
  return running_header;
}

bool T2PReferenceArea::suppress_headers()
// Returns true if there's any flag in this page that the headers should be suppressed.
{
  bool suppress = false;
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    T2PBlock *block = body_blocks[blk];
    if (block->suppress_header)
      suppress = true;
  }
  return suppress;
}

bool T2PReferenceArea::new_page_input_block_encountered(deque < T2PBlock * >&input_blocks, bool set_flag)
// Return true if the next input block starts a new page.
{
  if (!input_blocks.empty()) {
    if (input_blocks[0]->type == t2pbtNewPage) {
      if (!set_flag)
        return true;
      start_new_page = true;
    }
    if (input_blocks[0]->type == t2pbtNewOddPage) {
      if (!set_flag)
        return true;
      start_new_page = true;
      start_new_odd_page = true;
    }
  }
  return start_new_page;
}

void T2PReferenceArea::get_referents(map < ustring, ustring > &referents, unsigned int page_number)
// Gets the referents on this page, with their page numbers.
{
  for (unsigned int i = 0; i < body_blocks.size(); i++) {
    T2PBlock *block = body_blocks[i];
    if (!block->referent.empty()) {
      referents[block->referent] = convert_to_string(page_number);
    }
  }
}

void T2PReferenceArea::match_and_expand_references_and_referents(map < ustring, ustring > &referents)
// Writes the page numbers of the referents. Used in table of contents.
{
  for (unsigned int i = 0; i < body_blocks.size(); i++) {
    T2PBlock *block = body_blocks[i];
    if (!block->reference.empty()) {

      string referent = referents[block->reference];

      if (!referent.empty()) {

        PangoRectangle toc_rectangle;
        toc_rectangle.x = block->rectangle.x + block->rectangle.width;
        toc_rectangle.y = block->rectangle.y;
        toc_rectangle.width = block->maximum_width_pango_units;
        toc_rectangle.height = 0;

        T2PLayoutContainer *page_number_layout = new T2PLayoutContainer(toc_rectangle, NULL, cairo);
        table_of_contents_layout_containers.push_back(page_number_layout);
        page_number_layout->layout_text(NULL, 0, referent);
        page_number_layout->rectangle.x = block->rectangle.x + block->maximum_width_pango_units - page_number_layout->rectangle.width;

        int dot_distance_pango_units = millimeters_to_pango_units(2.5);
        int available_width = block->maximum_width_pango_units - block->rectangle.x - block->rectangle.width - dot_distance_pango_units - dot_distance_pango_units - page_number_layout->rectangle.width;
        int start_x = block->rectangle.x + block->rectangle.width;

        while (available_width > 0) {
          T2PLayoutContainer *dotted_line_layout = new T2PLayoutContainer(toc_rectangle, NULL, cairo);
          table_of_contents_layout_containers.push_back(dotted_line_layout);
          string dot(".");
          dotted_line_layout->layout_text(NULL, 0, dot);
          dotted_line_layout->rectangle.x = start_x + dot_distance_pango_units;
          start_x += (dotted_line_layout->rectangle.width + dot_distance_pango_units);
          available_width -= (dotted_line_layout->rectangle.width + dot_distance_pango_units);
        }
      }
    }
  }
}
