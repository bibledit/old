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

T2PReferenceArea::T2PReferenceArea(PangoRectangle rectangle_in) :
  T2PArea(rectangle_in)
// This is a reference area, e.g. a header, footer, or text area.
{
}

T2PReferenceArea::~T2PReferenceArea()
// Destructor.
{
  for (unsigned int i = 0; i < blocks.size(); i++) {
    delete blocks[i];
  }
}

void T2PReferenceArea::add_block(T2PBlock * block) {
  blocks.push_back(block);
}

void T2PReferenceArea::print(cairo_t *cairo)
// Print the blocks.
{
  // Go through each block.
  for (unsigned int blk = 0; blk < blocks.size(); blk++) {
    T2PBlock * block = blocks[blk];
    // While laying out text, the block's x and y values are relative to the parent's values.
    // For printing this needs to be adjusted.
    block->rectangle.x += rectangle.x;
    block->rectangle.y += rectangle.y;
    // Print this block.
    block->print(cairo);
  }
}

void T2PReferenceArea::fit_blocks(deque <T2PBlock *>& input_blocks)
// Fits the blocks into the reference area.
{
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

  // Any remaining blocks with equal column count left, re-insert these into the input blocks.
  for (int i = blocks_with_equal_column_count.size() - 1; i >= 0; i--) {
    input_blocks.push_front(blocks_with_equal_column_count[i]);
  }
}

void T2PReferenceArea::fit_column(deque <T2PBlock *>& input_blocks)
// Tries to fit in a group of blocks that has the same column count.
{
  // Bail out if there's no input.
  if (input_blocks.empty())
    return;

  // If the stacked y is already crossing the height of the reference area, bail out.
  if (fitted_blocks_height_pango_units() > rectangle.height)
    return;

  // Fit the column(s) in.
  fit_columns(input_blocks, input_blocks[0]->column_count);
}

void T2PReferenceArea::fit_columns(deque <T2PBlock *>& input_blocks, int column_count)
// Fits the input blocks into one or two columns.
{
  // Bail out if there's no input.
  if (input_blocks.empty())
    return;

  // Initialize variables.
  int start_stacked_y = fitted_blocks_height_pango_units();
  int current_column_number = -1;
  int column_stacked_y = rectangle.height + 1;
  int second_column_number_x = rectangle.width - input_blocks[0]->rectangle.width;
  unsigned int blocks_size_before_fitting_columns = blocks.size();

  // If the height of the already fitted blocks plus the first one clips against the height of the reference area, bail out.
  if ((start_stacked_y + input_blocks[0]->rectangle.height) > rectangle.height)
    return;

  // Go through the available blocks to fit them in and sort them out.
  while (!input_blocks.empty()) {

    // Point to the block we deal with.
    T2PBlock * block = input_blocks[0];

    // If the stacked y value would cross the maximum height, switch to the next column.
    if ((column_stacked_y + block->rectangle.height) > rectangle.height) {
      // If there's no next column, it means that the desired number of columns have been filled: bail out.
      // It also means that the column balancing won't be done, which is good, because
      // it makes it faster.
      current_column_number++;
      if (current_column_number >= column_count) {
        return;
      }
      column_stacked_y = start_stacked_y;
    }

    // The y value of the block will be calculated based on stacked blocks before.
    block->rectangle.y = column_stacked_y;

    // Set the x value of the block.
    if (current_column_number == 0) {
      block->rectangle.x = 0;
    } else {
      block->rectangle.x = second_column_number_x;
    }

    // New stacked y for next iteration.
    column_stacked_y += block->rectangle.height;

    // Block fits: move it in the object.
    blocks.push_back(block);
    input_blocks.erase(input_blocks.begin());
  }

  // Balance the columns if there are two columns.
  if (column_count > 1) {
    balance_two_columns(blocks_size_before_fitting_columns, start_stacked_y);
  }
}

int T2PReferenceArea::fitted_blocks_height_pango_units()
// This returns the maximum height of the blocks that have already been fitted in the reference area.
// There are blocks that form a column, so we can't just take the height of the last fitted block,
// but have to go through all the blocks to see whatever biggest height we can get.
{
  int maximum_height = 0;
  for (unsigned int blk = 0; blk < blocks.size(); blk++) {
    int y_plus_height = blocks[blk]->rectangle.y + blocks[blk]->rectangle.height;
    if (y_plus_height > maximum_height)
      maximum_height = y_plus_height;
  }
  return maximum_height;
}

void T2PReferenceArea::balance_two_columns(int start_block, int start_height)
// Balance two columns starting from "start_block" going to the end.
{
  // If there aren't enough blocks for balancing, bail out.
  if (blocks.size() - start_block < 2)
    return;

  // Initialize variables.
  int second_column_number_x = rectangle.width - blocks[0]->rectangle.width;

  // Store balancing points and balanced heights. 
  vector <unsigned int> balancing_points;
  vector <unsigned int> balanced_heights;

  // Try all balancing points and store these and their associated heights.
  for (unsigned int balancing_point = start_block; balancing_point < blocks.size(); balancing_point++) {
    int column_one_y = start_height;
    int column_two_y = start_height;
    for (unsigned int blk = start_block; blk < blocks.size(); blk++) {
      if (blk > balancing_point) {
        column_two_y += blocks[blk]->rectangle.height;
      } else {
        column_one_y += blocks[blk]->rectangle.height;
      }
    }
    balancing_points.push_back(balancing_point);
    balanced_heights.push_back(MAX (column_one_y, column_two_y));
  }

  // Look for the balancing point that has the smallest height.
  unsigned int balancing_point_with_smallest_height = 0;
  unsigned int smallest_height= INT_MAX;
  for (unsigned int i = 0; i < balancing_points.size(); i++) {
    if (balanced_heights[i] <= smallest_height) {
      balancing_point_with_smallest_height = balancing_points[i];
      smallest_height = balanced_heights[i];
    }
  }

  // Balance the column at the optimal balancing point.
  int column_two_y = start_height;
  for (unsigned int i = 0; i < balancing_points.size(); i++) {
    if (i + start_block > balancing_point_with_smallest_height) {
      blocks[i + start_block]->rectangle.x = second_column_number_x;
      blocks[i + start_block]->rectangle.y = column_two_y;
      column_two_y += blocks[i + start_block]->rectangle.height;
    }
  }
}

