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
#include <glib.h>
#include "windowoutline.h"
#include "help.h"
#include "keyterms.h"
#include "tiny_utilities.h"


WindowOutline::WindowOutline(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup):
FloatingWindow (parent_layout, widOutline, "Outline", startup), myreference(0)
// Window showing outline.
{
  // Outline object.
  outline = new Outline(vbox_client);

  // Focus control.
  last_focused_widget = outline->treeview;
  gtk_widget_grab_focus (last_focused_widget);
  connect_focus_signals (outline->treeview);
}


WindowOutline::~WindowOutline()
{
  delete outline;
}


void WindowOutline::go_to(const ustring & project, Reference & reference)
{
  // Bail out if there's no change in the reference.
  if (myreference.equals(reference))
    return;

  // Go to the reference.
  outline->goto_reference(project, reference);

  // Store the new reference in the object.
  myreference.assign(reference);
}


