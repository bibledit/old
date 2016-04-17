/*
 ** Copyright (©) 2016 Matt Postiff.
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

// Simple "glue" to connect our code to the glibmm ustring utilities. Teus 
// formerly had a copy of the glibmm/ustring code here. We'd rather use 
// the existing library and not have to maintain it, and also benefit from 
// improvements they make in it.
 
#ifndef INCLUDED_USTRING_H
#define INCLUDED_USTRING_H

#include <glibmm/ustring.h>
using namespace Glib;

#endif
