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


#include "utilities.h"
#include <glib.h>
#include "resource.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "moziller.h"
#include "resource_utils.h"


Resource::Resource (GtkWidget * vbox, GtkWidget * notebook_page, GtkWidget * tab_label)
{
  // Save and initialize varables.
  my_vbox = vbox;
  my_notebook_page = notebook_page;
  my_tab_label = tab_label;
  focus_set = false;
  loading = false;
  resource_type = rtEnd;
  
  // Build gui.
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  progressbar = gtk_progress_bar_new ();
  gtk_widget_show (progressbar);
  gtk_box_pack_start (GTK_BOX (hbox), progressbar, TRUE, TRUE, 0);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), "");
  gtk_progress_bar_set_ellipsize (GTK_PROGRESS_BAR (progressbar), PANGO_ELLIPSIZE_MIDDLE);

  animation = gtk_image_new_from_file (gw_build_filename (directories_get_package_data (), "animation.gif").c_str());
  gtk_widget_show (animation);
  gtk_box_pack_start (GTK_BOX (hbox), animation, FALSE, FALSE, 0);

  mozembed = gtk_moz_embed_new ();
  gtk_widget_show (mozembed);
  gtk_box_pack_start (GTK_BOX (vbox), mozembed, TRUE, TRUE, 0);

  g_signal_connect ((gpointer) progressbar, "button_press_event", G_CALLBACK (on_progressbar_button_press_event), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "link_message", G_CALLBACK (on_link_message_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "location", G_CALLBACK (on_location_changed_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "progress", G_CALLBACK (on_progress_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "progress_all", G_CALLBACK (on_progress_all_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "net_start", G_CALLBACK (on_load_started_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "net_stop", G_CALLBACK (on_load_finished_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "js_status", G_CALLBACK (on_js_status_changed_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "visibility", G_CALLBACK (on_visibility_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "size_to", G_CALLBACK (on_size_to_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_key_down", G_CALLBACK (on_dom_key_down_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_key_press", G_CALLBACK (on_dom_key_press_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_key_up", G_CALLBACK (on_dom_key_up_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_mouse_down", G_CALLBACK (on_dom_mouse_down_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_mouse_up", G_CALLBACK (on_dom_mouse_up_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_mouse_click", G_CALLBACK (on_dom_mouse_click_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_mouse_dbl_click", G_CALLBACK (on_dom_mouse_dbl_click_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_mouse_over", G_CALLBACK (on_dom_mouse_over_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_mouse_out", G_CALLBACK (on_dom_mouse_out_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "security_change", G_CALLBACK (on_security_change_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "status_change", G_CALLBACK (on_status_change_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_activate", G_CALLBACK (on_dom_activate_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_focus_in", G_CALLBACK (on_dom_focus_in_cb), gpointer(this));
  g_signal_connect ((gpointer) mozembed, "dom_focus_out", G_CALLBACK (on_dom_focus_out_cb), gpointer(this));

  gtk_widget_grab_focus (hbox);
  gtk_widget_hide (animation);
  
  // Create focus signal.
  focus_signal = gtk_button_new ();
  focus ();
}


Resource::~Resource ()
{
  gtk_widget_destroy (focus_signal);
  gtk_widget_destroy (my_vbox);
}


void Resource::focus ()
{
  gtk_button_clicked (GTK_BUTTON (focus_signal));
  focus_set = true;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 1);
  gtk_label_set_text (GTK_LABEL (my_tab_label), title.c_str());
}


bool Resource::focused ()
{
  return focus_set;
}


void Resource::defocus ()
{
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  focus_set = false;
}


void Resource::parent_notebook_switches_to_page (GtkWidget * page)
{
  if (page == my_notebook_page) {
    focus ();
  }
}


void Resource::copy ()
{
  moziller_copy_selection_to_clipboard (mozembed);
}


bool Resource::can_go_back ()
{
  return gtk_moz_embed_can_go_back (GTK_MOZ_EMBED (mozembed));
}


bool Resource::can_go_forward ()
{
  return gtk_moz_embed_can_go_forward (GTK_MOZ_EMBED (mozembed));
}


void Resource::go_back ()
{
  gtk_moz_embed_go_back (GTK_MOZ_EMBED (mozembed));
}


void Resource::go_forward ()
{
  gtk_moz_embed_go_forward (GTK_MOZ_EMBED (mozembed));
}


void Resource::reload ()
{
  gtk_moz_embed_reload (GTK_MOZ_EMBED (mozembed), GTK_MOZ_EMBED_FLAG_RELOADNORMAL);
}


void Resource::stop ()
{
  gtk_moz_embed_stop_load (GTK_MOZ_EMBED (mozembed));
}


void Resource::home ()
{
  gtk_moz_embed_load_url (GTK_MOZ_EMBED (mozembed), homepage.c_str ());
}


void Resource::go_to (const Reference& reference)
{
  ustring url;
  switch (resource_type) {
    case rtForeignDataURLForEachVerse:
    case rtURLForEachVerse:
    {
      url = resource_construct_url (url_structure, book_renderer, reference);
      break;
    }
    case rtIndexFileOnFlyForEachVerseWithDifferentAnchors:
    {
      url = resource_construct_index_file (gw_path_get_dirname (mytemplatefile), index_file_structure, book_renderer, anchor_renderer, reference, false);
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
  gtk_moz_embed_load_url (GTK_MOZ_EMBED (mozembed), url.c_str ());
}


gboolean Resource::on_progressbar_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  // Clicking on the progressbar does not call this event.
  return ((Resource *) user_data)->on_progressbar_button_press (event);
}


gboolean Resource::on_progressbar_button_press (GdkEventButton *event)
{
  focus ();
  return false;
}


void Resource::open (const ustring& filename)
{
  mytemplatefile = filename;
  resource_type = resource_get_type (filename);
  homepage = resource_url_modifier (resource_get_home_page (filename), resource_type, filename);
  url_structure = resource_url_modifier (resource_get_url_constructor (filename), resource_type, filename);
  index_file_structure = resource_get_index_file_constructor (filename);
  book_renderer = resource_get_books (filename);
  anchor_renderer = resource_get_anchors (filename);
  home ();
  title = resource_get_title (filename);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), title.c_str());
}


void Resource::on_location_changed_cb (GtkMozEmbed *embed, gpointer data)
{
  ((Resource *) data)->on_location_changed ();
}


void Resource::on_location_changed ()
{
  gchar * location = gtk_moz_embed_get_location (GTK_MOZ_EMBED (mozembed));
  if (location) {
    g_free (location);
  } else {
  }
}


void Resource::on_link_message_cb (GtkMozEmbed *embed, gpointer data)
{
  ((Resource *) data)->on_link_message ();
}


void Resource::on_link_message ()
{
  ustring message;
  gchar * link_message = gtk_moz_embed_get_link_message (GTK_MOZ_EMBED (mozembed));
  if (link_message) {
    message = link_message;
    g_free (link_message);
  }
  if (message.empty ())
    message = title;
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), message.c_str());
}


void Resource::on_progress_cb (GtkMozEmbed *embed, gint cur, gint max, gpointer data)
{
  ((Resource *) data)->on_progress (cur, max);
}


void Resource::on_progress (gint cur, gint max)
{
}


void Resource::on_progress_all_cb (GtkMozEmbed *embed, const char *aURI, gint cur, gint max, gpointer data)
{
  ((Resource *) data)->on_progress_all (cur, max);
}


void Resource::on_progress_all (gint cur, gint max)
{
  static gint previousmax = 0;
  static double factor = 1;
  if (max != previousmax) {
    previousmax = max;
    factor = 1;
  }
  double fraction = 0.5;
  if (max != -1) fraction = (double) cur / (double) max * factor;
  if (fraction > 1) {
    factor *= 0.9;
    fraction *= 0.9;
  }
  if (fraction > 1) {
    fraction = 1;
  }
  if (fraction < 0) fraction = 0;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), fraction);
}


void Resource::on_load_started_cb (GtkMozEmbed *embed, gpointer data)
{
  ((Resource *) data)->on_load_started ();
}


void Resource::on_load_started ()
{
  gtk_widget_show (animation);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  loading = true;
}


void Resource::on_load_finished_cb (GtkMozEmbed *embed, gpointer data)
{
  ((Resource *) data)->on_load_finished ();
}


void Resource::on_load_finished ()
{
  gtk_widget_hide (animation);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), focus_set ? 1 : 0);
  loading = false;
}


void Resource::on_new_window_cb (GtkMozEmbed *embed, GtkMozEmbed **retval, guint chromemask, gpointer data)
{
  ((Resource *) data)->on_new_window (retval);
}


void Resource::on_new_window (GtkMozEmbed **retval)
{
}


void Resource::on_destroy_brsr_cb (GtkMozEmbed *embed, gpointer data)
{
  ((Resource *) data)->on_destroy_brsr (embed);
}


void Resource::on_destroy_brsr (GtkMozEmbed *embed)
{
}


gint Resource::on_open_uri_cb (GtkMozEmbed *embed, const char *uri, gpointer data)
{
  return ((Resource *) data)->on_open_uri (uri);
}


gint Resource::on_open_uri (const char *uri)
{
  return false;
}


void Resource::on_js_status_changed_cb (GtkMozEmbed *embed, gpointer data)
{
  ((Resource *) data)->on_js_status_changed ();
}


void Resource::on_js_status_changed ()
{
}


void Resource::on_visibility_cb (GtkMozEmbed *embed, gboolean visibility, gpointer data)
{
  ((Resource *) data)->on_visibility (visibility);
}


void Resource::on_visibility (gboolean visibility)
{
}


void Resource::on_size_to_cb (GtkMozEmbed *embed, gint width, gint height, gpointer data)
{
  ((Resource *) data)->on_size_to (width, height);
}


void Resource::on_size_to (gint width, gint height)
{
}


gint Resource::on_dom_key_down_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_key_down (dom_event);
}


gint Resource::on_dom_key_down (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_key_press_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_key_press (dom_event);
}


gint Resource::on_dom_key_press (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_key_up_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_key_up (dom_event);
}


gint Resource::on_dom_key_up (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_mouse_down_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_mouse_down (dom_event);
}


gint Resource::on_dom_mouse_down (gpointer dom_event)
{
  focus ();
  return false;
}


gint Resource::on_dom_mouse_up_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_mouse_up (dom_event);
}


gint Resource::on_dom_mouse_up (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_mouse_click_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_mouse_click (dom_event);
}


gint Resource::on_dom_mouse_click (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_mouse_dbl_click_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_mouse_dbl_click (dom_event);
}


gint Resource::on_dom_mouse_dbl_click (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_mouse_over_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_mouse_over (dom_event);
}


gint Resource::on_dom_mouse_over (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_mouse_out_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_mouse_out (dom_event);
}


gint Resource::on_dom_mouse_out (gpointer dom_event)
{
  return false;
}


void Resource::on_security_change_cb (GtkMozEmbed *embed, gpointer request, guint state, gpointer data)
{
  ((Resource *) data)->on_security_change (request, state);
}


void Resource::on_security_change (gpointer request, guint state)
{
}


void Resource::on_status_change_cb (GtkMozEmbed *embed, gpointer request, gint status, gpointer message, gpointer data)
{
  ((Resource *) data)->on_status_change (request, status, message);
}


void Resource::on_status_change (gpointer request, gint status, gpointer message)
{
}


gint Resource::on_dom_activate_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_activate (dom_event);
}


gint Resource::on_dom_activate (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_focus_in_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_focus_in (dom_event);
}


gint Resource::on_dom_focus_in (gpointer dom_event)
{
  return false;
}


gint Resource::on_dom_focus_out_cb (GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
  return ((Resource *) data)->on_dom_focus_out (dom_event);
}


gint Resource::on_dom_focus_out (gpointer dom_event)
{
  return false;
}


ustring Resource::template_get ()
{
  return mytemplatefile;
}


GtkWidget * Resource::parent_notebook_page ()
{
  return my_notebook_page;
}
