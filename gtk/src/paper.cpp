 /*
  ** Copyright (©) 2003-2013 Teus Benschop.
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

#include "libraries.h"
#include "paper.h"

const char *PAPER_SIZES[NUMBER_OF_PAPERSIZES] = { "A3", "A4", "A5",
  "B4 (ISO)", "B5 (ISO)", "B6 (ISO)",
  "B4 (JIS)", "B5 (JIS)", "B6 (JIS)",
  "Letter", "Legal", "Tabloit",
  "C4", "C5", "C6"
};

double PAPER_WIDTHS[NUMBER_OF_PAPERSIZES] = { 29.70, 21.00, 14.80,
  25.00, 17.60, 12.50,
  25.70, 18.20, 12.80,
  21.59, 21.59, 27.96,
  22.90, 16.20, 11.40
};

double PAPER_HEIGHTS[NUMBER_OF_PAPERSIZES] = { 42.00, 29.70, 21.00,
  35.30, 25.00, 17.60,
  36.40, 25.70, 18.20,
  27.94, 35.57, 43.13,
  32.40, 22.90, 16.20
};

ustring paper_size_get_name(int index)
{
  ustring value;
  if (index < NUMBER_OF_PAPERSIZES)
    value = PAPER_SIZES[index];
  else
    value = "User defined";
  return value;
}

ustring paper_size_get_name(double width, double height)
// Given a width and a height, return the papersize, e.g. A4.
// If no size is found, return "User defined".
{
  ustring result = paper_size_get_name(NUMBER_OF_PAPERSIZES);
  for (unsigned int i = 0; i < NUMBER_OF_PAPERSIZES; i++)
    if (width == PAPER_WIDTHS[i])
      if (height == PAPER_HEIGHTS[i])
        result = paper_size_get_name(i);
  return result;
}

double paper_size_get_width(const ustring & size)
// Returns the width belonging to a size, e.g. A4 returns 21.00
{
  for (unsigned int i = 0; i < NUMBER_OF_PAPERSIZES; i++)
    if (size == paper_size_get_name(i))
      return PAPER_WIDTHS[i];
  return 0;
}

double paper_size_get_height(const ustring & size)
// Returns the height belonging to a size, e.g. A4 returns 29.70
{
  for (unsigned int i = 0; i < NUMBER_OF_PAPERSIZES; i++)
    if (size == paper_size_get_name(i))
      return PAPER_HEIGHTS[i];
  return 0;
}
