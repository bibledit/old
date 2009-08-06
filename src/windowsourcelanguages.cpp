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
#include "windowsourcelanguages.h"
#include "help.h"
#include "window.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "projectutils.h"
#include "settings.h"
#include "keyboard.h"
#include "dialogentry3.h"
#include "gtkwrappers.h"
#include "referenceutils.h"
#include <sqlite3.h>
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "bible.h"
#include "usfmtools.h"
#include "dialogeditlist.h"
#include "swordkjv.h"


WindowSourceLanguages::WindowSourceLanguages(GtkAccelGroup * accelerator_group, bool startup, GtkWidget * parent_box):
WindowBase(widSourceLanguages, "Source languages", startup, 0, parent_box), reference(0, 0, "")
// Window for working with the source languages.  
{
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_container_add(GTK_CONTAINER(window_vbox), scrolledwindow);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  htmlview = gtk_html_new();
  gtk_widget_show(htmlview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), htmlview);
  gtk_html_allow_selection(GTK_HTML(htmlview), true);

  g_signal_connect((gpointer) htmlview, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));

  // Signal button.
  signal_button = gtk_button_new();

  // Main focused widget.
  last_focused_widget = htmlview;
  gtk_widget_grab_focus (last_focused_widget);
}


WindowSourceLanguages::~WindowSourceLanguages()
{
  // Destroy signal button.
  gtk_widget_destroy(signal_button);
}


void WindowSourceLanguages::go_to (const Reference& reference_in, const ustring& project_in)
{
  reference = reference_in;
  project = project_in;
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  language = projectconfig->language_get();
  html_link_clicked ("");
}


gboolean WindowSourceLanguages::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((WindowSourceLanguages *) user_data)->html_link_clicked(url);
  return true;
}


void WindowSourceLanguages::html_link_clicked (const gchar * url)
// Callback for clicking a link.
{
  // Store scrolling position for the now active url.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  scrolling_position[active_url] = gtk_adjustment_get_value (adjustment);

  // Handle the back button and url mechanism.
  bool store_url = true;
  if (strcmp (url, "back") == 0) {
    if (url_stack.size() > 1) {
      active_url = url_stack[url_stack.size() - 2];
      store_url = false;
      url_stack.pop_back ();
    }
  } else {
    active_url = url;
  }

  // Maximum size of the url stack.
  if (url_stack.size() > 50) {
    url_stack.pop_front ();
  }

  // Start writing a html page.
  HtmlWriter2 htmlwriter ("");
  bool display_another_page = true;

  if (active_url.find ("goto ") == 0) {
    // Signal that a reference was clicked.
    ustring ref (active_url);
    ref.erase (0, 5);
    //reference.assign (references[active_entry]);
    gtk_button_clicked(GTK_BUTTON(signal_button));
    display_another_page = false;
  }

  else if (active_url.find ("action") == 0) {
    // Go to the action page.
    html_write_action_page (htmlwriter);
  }

  else {
    // Load the references.
    html_write_home_page (htmlwriter);
  }
  
  htmlwriter.finish();
  if (display_another_page) {
    // Load the page.
    GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview));
    gtk_html_write(GTK_HTML(htmlview), stream, htmlwriter.html.c_str(), -1);
    gtk_html_end(GTK_HTML(htmlview), stream, GTK_HTML_STREAM_OK);
    // Scroll to the position that possibly was stored while this url was last active.
    gtk_adjustment_set_value (adjustment, scrolling_position[active_url]);
    // Store the url on the stack.
    if (store_url) {
      url_stack.push_back (active_url);
    }
  }
}


void WindowSourceLanguages::html_write_home_page (HtmlWriter2& htmlwriter)
{
  // Write action bar.
  html_write_action_bar (htmlwriter, true);
  htmlwriter.paragraph_open ();
  htmlwriter.text_add (reference.human_readable (language));
  htmlwriter.paragraph_close ();
}


void WindowSourceLanguages::html_write_action_bar (HtmlWriter2& htmlwriter, bool topbar)
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("actions", "[actions]");
  htmlwriter.paragraph_close ();
}


void WindowSourceLanguages::html_write_action_page (HtmlWriter2& htmlwriter)
{
  // Write the link for going back to the references.
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("back", "[back]");
  htmlwriter.text_add (" ");
  htmlwriter.hyperlink_add ("", "[home]");
  htmlwriter.paragraph_close ();
}



/*


Todo original language resource.















*/
