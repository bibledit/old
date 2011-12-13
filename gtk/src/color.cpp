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

#include "libraries.h"
#include "color.h"
#include "utilities.h"

void color_decimal_to_gdk(unsigned int colour, GdkColor * color)
{
  guint16 red, green, blue;
  blue = colour % 256;
  colour = colour / 256;
  green = colour % 256;
  colour = colour / 256;
  red = colour;
  color->red = 256 * red;
  color->green = 256 * green;
  color->blue = 256 * blue;
}

unsigned int color_gdk_to_decimal(GdkColor * color)
{
  unsigned int result;
  result = (65536 * (color->red / 256)) + (256 * (color->green / 256)) + (color->blue / 256);
  return result;
}

ustring color_decimal_to_hex(unsigned int colour)
{
  gchar *s = g_strdup_printf("%06X", colour);
  ustring result(s);
  g_free(s);
  return result;
}

void color_widget_default(GtkWidget * widget)
// Set the widget to the default color.
{
  gtk_widget_modify_base(widget, GTK_STATE_NORMAL, NULL);
  gtk_widget_modify_base(widget, GTK_STATE_SELECTED, NULL);
  gtk_widget_modify_base(widget, GTK_STATE_ACTIVE, NULL);
  gtk_widget_modify_text(widget, GTK_STATE_SELECTED, NULL);
  gtk_widget_modify_text(widget, GTK_STATE_ACTIVE, NULL);
  gtk_widget_modify_text(widget, GTK_STATE_NORMAL, NULL);
  // See the gedit source for this function.
  //modify_cursor_color (widget, NULL); 
}

void color_widget_set(GtkWidget * widget, unsigned int normal_text, unsigned int background, unsigned int selected_text, unsigned int selection)
{
  GdkColor gdkcolor;
  color_decimal_to_gdk(normal_text, &gdkcolor);
  gtk_widget_modify_text(widget, GTK_STATE_NORMAL, &gdkcolor);
  //modify_cursor_color (widget, &gdkcolor);
  color_decimal_to_gdk(background, &gdkcolor);
  gtk_widget_modify_base(widget, GTK_STATE_NORMAL, &gdkcolor);
  color_decimal_to_gdk(selected_text, &gdkcolor);
  gtk_widget_modify_text(widget, GTK_STATE_SELECTED, &gdkcolor);
  gtk_widget_modify_text(widget, GTK_STATE_ACTIVE, &gdkcolor);
  color_decimal_to_gdk(selection, &gdkcolor);
  gtk_widget_modify_base(widget, GTK_STATE_SELECTED, &gdkcolor);
  gtk_widget_modify_base(widget, GTK_STATE_ACTIVE, &gdkcolor);
}
