/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "libraries.h"
#include <glib.h>
#include "windowoutline.h"
#include "help.h"
#include "windows.h"
#include "keyterms.h"
#include "tiny_utilities.h"

WindowOutline::WindowOutline(GtkAccelGroup *accelerator_group, bool startup) :
  WindowBase(widOutline, "Outline", accelerator_group, startup), myreference(0)
// Window showing outline.
{
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  
  outline = new Outline (vbox);
  g_signal_connect ((gpointer) outline->treeview, "visibility-notify-event", G_CALLBACK (on_visibility_notify_event), gpointer(this));

}

WindowOutline::~WindowOutline() {
  if (outline)
    delete outline;
}

void WindowOutline::go_to(const ustring& project, Reference& reference) {
  // Bail out if there's no change in the reference.
  if (myreference.equals(reference))
    return;

  // Go to the reference.
  outline->goto_reference(project, reference);

  // Store the new reference in the object.
  myreference.assign(reference);
}

