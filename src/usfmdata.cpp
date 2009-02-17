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

#include "usfmdata.h"

/*
*/
typeof(usfm_record) usfm_table[] =
{
  // Identification.
  {"id", true, false, 0},
  {"ide", true, false, 0},
  {"sts", true, false, 0},
  {"rem", true, false, 0},
  {"h", true, false, 0},
  {"h1", true, false, 0},
  {"h2", true, false, 0},
  {"h3", true, false, 0},
  {"toc1", true, false, 0},
  {"toc2", true, false, 0},
  {"toc3", true, false, 0},

  // Introductions.
  {"imt", true, false, 0},
  {"imt1", true, false, 0},
  {"imt2", true, false, 0},
  {"imt3", true, false, 0},
  {"imt4", true, false, 0},
  {"is", true, false, 0},
  {"is1", true, false, 0},
  {"is2", true, false, 0},
  {"is3", true, false, 0},
  {"is4", true, false, 0},
  {"ip", true, false, 0},
  {"ipi", true, false, 0},
  {"im", true, false, 0},
  {"imi", true, false, 0},
  {"ipq", true, false, 0},
  {"imq", true, false, 0},
  {"ipr", true, false, 0},
  {"iq", true, false, 0},
  {"iq1", true, false, 0},
  {"iq2", true, false, 0},
  {"iq3", true, false, 0},
  {"iq4", true, false, 0},
  {"ib", true, false, 0},
  {"ili", true, false, 0},
  {"iot", true, false, 0},
  {"io", true, false, 0},
  {"io1", true, false, 0},
  {"io2", true, false, 0},
  {"io3", true, false, 0},
  {"io4", true, false, 0},
  {"ior", false, false, 1},
  {"iex", true, false, 0},
  {"iqt", false, false, 1},
  {"imte", true, false, 0},
  {"ie", true, false, 0},

  // Titles, headings, and labels.
  {"mt", true, false, 0},
  {"mt1", true, false, 0},
  {"mt2", true, false, 0},
  {"mt3", true, false, 0},
  {"mt4", true, false, 0},
  {"mte", true, false, 0},
  {"mte1", true, false, 0},
  {"mte2", true, false, 0},
  {"mte3", true, false, 0},
  {"mte4", true, false, 0},
  {"ms", true, false, 0},
  {"ms1", true, false, 0},
  {"ms2", true, false, 0},
  {"ms3", true, false, 0},
  {"ms4", true, false, 0},
  {"mr", true, false, 0},
  {"s", true, false, 0},
  {"s1", true, false, 0},
  {"s2", true, false, 0},
  {"s3", true, false, 0},
  {"s4", true, false, 0},
  {"sr", true, false, 0},
  {"r", true, false, 0},
  {"rq", false, false, 1},
  {"d", true, false, 0},
  {"sp", true, false, 0},
  


  // The end.
  {"", false, false, 0}
};

unsigned int usfmdata_markers_count()
{
  unsigned int count = sizeof(usfm_table) / sizeof(*usfm_table);
  return count;
}
