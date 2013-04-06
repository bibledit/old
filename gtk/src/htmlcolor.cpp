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


#include <config.h>
#include "htmlcolor.h"


/*
// Todo 
HTMLColor *html_color_new(void)
{
  HTMLColor *nc = g_new0(HTMLColor, 1);

  nc->refcount = 1;

  return nc;
}


HTMLColor *html_color_new_from_gdk_color(const GdkColor * color)
{
  HTMLColor *nc = html_color_new();

  nc->color = *color;

  return nc;
}


HTMLColor *html_color_new_from_rgb(gushort red, gushort green, gushort blue)
{
  HTMLColor *nc = html_color_new();

  nc->color.red = red;
  nc->color.green = green;
  nc->color.blue = blue;

  return nc;
}


void html_color_ref(HTMLColor * color)
{
  g_assert(color);

  color->refcount++;
}


void html_color_unref(HTMLColor * color)
{
  g_assert(color);
  g_assert(color->refcount > 0);

  color->refcount--;

  if (!color->refcount) {
    // if (color->allocated)
    //   g_warning ("FIXME, color free\n");
    // FIXME commented out to catch g_asserts on refcount so we could hunt "too much unrefs" bugs
    g_free(color);
  }
}

*/
