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

#include "libraries.h"
#include <glib.h>
#include "windowoutline.h"
#include "help.h"
#include "window.h"
#include "keyterms.h"
#include "tiny_utilities.h"

WindowOutline::WindowOutline(GtkAccelGroup * accelerator_group, bool startup, GtkWidget * parent_box):
WindowBase(widOutline, "Outline", startup, 0, parent_box), myreference(0)
// Window showing outline.
{
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox);
  gtk_container_add(GTK_CONTAINER(window_vbox), vbox);

  outline = new Outline(vbox);
  g_signal_connect((gpointer) outline->treeview, "visibility-notify-event", G_CALLBACK(on_visibility_notify_event), gpointer(this));

  // Main focused widget.
  last_focused_widget = outline->treeview;
  gtk_widget_grab_focus (last_focused_widget);
}

WindowOutline::~WindowOutline()
{
  if (outline)
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
