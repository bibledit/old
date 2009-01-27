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

// The code was copied from software written and copyrighted by the gtkmm Development Team


#ifndef INCLUDED_USTRING_GLIB_H
#define INCLUDED_USTRING_GLIB_H

#include "ustring.h"

enum GlibNormalizeMode
{
  NORMALIZE_DEFAULT,
  NORMALIZE_NFD = NORMALIZE_DEFAULT,
  NORMALIZE_DEFAULT_COMPOSE,
  NORMALIZE_NFC = NORMALIZE_DEFAULT_COMPOSE,
  NORMALIZE_ALL,
  NORMALIZE_NFKD = NORMALIZE_ALL,
  NORMALIZE_ALL_COMPOSE,
  NORMALIZE_NFKC = NORMALIZE_ALL_COMPOSE
};

#endif /* INCLUDED_USTRING_GLIB_H */

