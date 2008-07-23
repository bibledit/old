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
#include "text2pdf_ref_area.h"
#include "gwrappers.h"

T2PReferenceArea::T2PReferenceArea(PangoRectangle rectangle_in) :
  T2PArea(rectangle_in)
// This is a reference area, e.g. a header, footer, or text area.
{
  // Initialize the y value to start stacking blocks at.
  start_stacking_y = 0;
  column_spacing_pango_units = 0;
}

T2PReferenceArea::~T2PReferenceArea()
// Destructor.
{
  for (unsigned int i = 0; i < body_blocks.size(); i++) {
    delete body_blocks[i];
  }
}

void T2PReferenceArea::print(cairo_t *cairo)
// Print the blocks.
{
  // Go through each block.
  for (unsigned int blk = 0; blk < body_blocks.size(); blk++) {
    T2PBlock * block = body_blocks[blk];
    // While laying out text, the block's x and y values are relative to the parent's values.
    // For printing this needs to be adjusted.
    block->rectangle.x += rectangle.x;
    block->rectangle.y += rectangle.y;
    // Print this block.
    block->print(cairo);
  }
}

void T2PReferenceArea::fit_blocks(deque <T2PBlock *>& input_blocks, int column_spacing_pango_units_in)
// Fits the blocks into the reference area.
{
  // Store column spacing.
  column_spacing_pango_units = column_spacing_pango_units_in;
  // Deal with the blocks after grouping them by equal column count.
  deque <T2PBlock *> blocks_with_equal_column_count;
  int n_columns = 0;
  while (!input_blocks.empty()) {
    if (input_blocks[0]->column_count != n_columns) {
      fit_column(blocks_with_equal_column_count);
      if (!blocks_with_equal_column_count.empty())
        break;
      n_columns = input_blocks[0]->column_count;
    }
    blocks_with_equal_column_count.push_back(input_blocks[0]);
    input_blocks.erase(input_blocks.begin());
  }
  fit_column(blocks_with_equal_column_count);

  // Re-insert any unfitted remaining blocks with equal column count into the input blocks.
  for (int i = blocks_with_equal_column_count.size() - 1; i >= 0; i--) {
    input_blocks.push_front(blocks_with_equal_column_count[i]);
  }
  
  // Look for any last blocks on the page that have their "keep_with_next" property set.
  // If these are there, that means these should be removed from here, and be put back into 
  // the input stream, so that they can be kept with paragraphs on the next page.
  while (!body_blocks.empty() && body_blocks[body_blocks.size()-1]->keep_with_next) {
    input_blocks.push_front(body_blocks[body_blocks.size()-1]);
    body_blocks.pop_back();
  }
}

void T2PReferenceArea::fit_column(deque <T2PBlock *>& input_blocks)
// Tries to fit in a group of blocks that has the same column count.
{
  // Bail out if there's no input.
  if (input_blocks.empty())
    return;

  // Bail out if the reference area is already fully stacked.
  if (start_stacking_y >= rectangle.height)
    return;

  // Fit the column(s) in.
  fit_columns(input_blocks, input_blocks[0]->column_count);
}

void T2PReferenceArea::fit_columns(deque <T2PBlock *>& input_blocks, int column_count)
/*
 Fits the input blocks into one or two columns.

 Fitting one column is easy: Just stack the blocks till the area is full.
 
 Fitting two columns requires a little more effort.
 The system described here tries to be fast and simple.
 If there area any input blocks available for fitting in, then instead of considering the 
 blocks one by one, it may happen that several blocks are considered at once.
 This happens if the T2PBlock->keep_with_next property is set.
 It tries to fit the next group of blocks into the second column.
 Then it keeps shifting blocks from the second column to the first one till the 
 height of the first colum is bigger than the height of the second one. 
 
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
  deque <T2PBigBlock> first_column;
  deque <T2PBigBlock> last_column;

  // Go through the input blocks.
  while (!input_blocks.empty()) {

    // Get the next lot of blocks that stay together.
    T2PBigBlock big_block = get_next_big_block_to_be_kept_together(input_blocks, column_count);

    // If the big block won't fit in the last column, then the area is full: bail out.
    int last_column_height = get_column_height (last_column, start_stacking_y);
    if ((start_stacking_y + last_column_height + big_block.height(start_stacking_y + last_column.size())) > rectangle.height) {
      break;
    }

    // The big block fits: store it in the last column.
    last_column.push_back(big_block);

    // Remove the fitted blocks from the input.
    for (unsigned int i = 0; i < big_block.blocks.size(); i++) {
      input_blocks.pop_front();
    }

    // Balance columns if there's more than one column.
    if (column_count > 1) {
      // The first column should be higher than the last, normally, except when there's no space.
      // Keep moving big blocks till that situation has been reached.
      bool last_column_is_higher_than_first = true;
      bool first_column_has_still_space = true;
      while (last_column_is_higher_than_first && first_column_has_still_space && !last_column.empty()) {
        int first_column_height = get_column_height (first_column, start_stacking_y);
        int last_column_height = get_column_height (last_column, start_stacking_y);
        last_column_is_higher_than_first = (last_column_height > first_column_height);
        if (last_column_is_higher_than_first) {
          T2PBigBlock first_big_block_of_second_column = last_column[0];
          int first_big_block_of_second_column_height = first_big_block_of_second_column.height (1);
          first_column_has_still_space = (rectangle.height - first_big_block_of_second_column_height - first_column_height) >= 0;
          if (first_column_has_still_space) {
            first_column.push_back (first_big_block_of_second_column);
            last_column.pop_front();
          }
        }
      }
    }
    
  }
  
  // Set the position of each individual block.
  // Copy the blocks from the columns into the object.
  int first_column_y = start_stacking_y;
  for (unsigned int i = 0; i < first_column.size(); i++) {
    first_column[i].set_blocks_x(0);
    int column_height = first_column[i].height(first_column_y); 
    first_column[i].set_blocks_y(first_column_y);
    first_column_y += column_height;
    for (unsigned int i2 = 0; i2 < first_column[i].blocks.size(); i2++) {
      body_blocks.push_back(first_column[i].blocks[i2]);
    }
  }
  int last_column_y = start_stacking_y;
  for (unsigned int i = 0; i < last_column.size(); i++) {
    if (last_column[i].column_count == 1) {
      last_column[i].set_blocks_x(0);
    } else {
      last_column[i].set_blocks_x(rectangle.width - ((rectangle.width - column_spacing_pango_units) / 2));
    }
    int column_height = last_column[i].height(last_column_y); 
    last_column[i].set_blocks_y(last_column_y);
    last_column_y += column_height;
    for (unsigned int i2 = 0; i2 < last_column[i].blocks.size(); i2++) {
      body_blocks.push_back(last_column[i].blocks[i2]);
    }
  }
  
  // Store maximum height of any column as the starting point for the next column.
  start_stacking_y = MAX (first_column_y, last_column_y);
}

T2PBigBlock T2PReferenceArea::get_next_big_block_to_be_kept_together(deque <T2PBlock *>& input_blocks, int column_count)
// Looks at "input_blocks", and gets the next blocks to be kept together.
// It does that by considering the "keep_with_next" property of T2PBlock.
// The big blocks returned could contain one block in the normal case, and more if these are kept
// together.
// It allocates the object, and the caller should free it.
{
  T2PBigBlock big_block(column_count);
  for (unsigned int i = 0; i < input_blocks.size(); i++) {
    big_block.blocks.push_back(input_blocks[i]);
    if (!input_blocks[i]->keep_with_next)
      break;
  }
  big_block.calculate_height();
  return big_block;
}

int T2PReferenceArea::get_column_height (deque <T2PBigBlock>& column, int reference_y)
// Gets the height of the column.
{
  int height = 0;
  for (unsigned int i = 0; i < column.size(); i++) {
    height += column[i].height(reference_y + i);
  }
  return height;
}
