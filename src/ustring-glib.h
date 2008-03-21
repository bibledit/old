/* Copyright (©) 2002 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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

