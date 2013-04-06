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


#ifndef _HTML_COLOR_H_
#define _HTML_COLOR_H_


#include <gdk/gdk.h>


struct _HTMLColor
{
  GdkColor color;
  gboolean allocated;
  guint refcount;
};


// Todo HTMLColor * html_color_new(void);
// Todo HTMLColor * html_color_new_from_gdk_color(const GdkColor *color);
// Todo HTMLColor * html_color_new_from_rgb(gushort red, gushort green, gushort blue);

// Todo void html_color_ref(HTMLColor *color);
// Todo void html_color_unref(HTMLColor *color);

// Todo void html_color_alloc(HTMLColor *color, HTMLPainter *painter);
// Todo void html_color_free(HTMLColor *color, HTMLPainter *painter);

// Todo gboolean html_color_equal(HTMLColor *color1, HTMLColor *color2);

// Todo void html_color_set(HTMLColor *color, GdkColor *c);


#endif
