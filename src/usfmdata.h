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

#ifndef INCLUDED_USFMDATA_H
#define INCLUDED_USFMDATA_H

typedef struct
{
  const char *marker; // The USFM.
  bool starts_new_line_usfm; // Whether the marker should start a new line in tidy USFM representation.
  bool starts_division_osis; // Whether the marker should start a new division in OSIS code.
  int has_end_marker; // Whether the marker has an endmarker. 0: No; 1: Yes; 2: Optional.
} usfm_record;

unsigned int usfmdata_markers_count();

#endif
