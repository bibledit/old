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
#include <glib.h>
#include "windowshowrelatedverses.h"
#include "help.h"
#include "floatingwindow.h"
#include "keyterms.h"
#include "tiny_utilities.h"
#include "kjv.h"
#include "parallel_passages.h"
#include "settings.h"
#include "gwrappers.h"


WindowShowRelatedVerses::WindowShowRelatedVerses(GtkWidget * parent_layout, GtkAccelGroup *accelerator_group, bool startup):
FloatingWindow(parent_layout, widShowRelatedVerses, "Related verses", startup), myreference(0)
// Window showing related verses.
{
  item_type = ritNone;
  thread_runs = false;
  event_id = 0;

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_container_add(GTK_CONTAINER(vbox_client), scrolledwindow);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_SHADOW_IN);

  htmlview = gtk_html_new();
  gtk_widget_show(htmlview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), htmlview);
  gtk_html_allow_selection(GTK_HTML(htmlview), true);

  connect_focus_signals (htmlview);
  
  g_signal_connect((gpointer) htmlview, "link-clicked", G_CALLBACK(on_html_link_clicked), gpointer(this));

  last_focused_widget = htmlview;
  gtk_widget_grab_focus (last_focused_widget);
  
  button_item = gtk_button_new ();
}


WindowShowRelatedVerses::~WindowShowRelatedVerses()
{
  gtk_widget_destroy (button_item);
  if (event_id) {
    gw_destroy_source (event_id);
  }
}


void WindowShowRelatedVerses::go_to(const ustring & project, const Reference & reference)
{
  if (!myreference.equals(reference)) {
    myreference.assign(reference);
    myproject = project;
    if (event_id) {
      gw_destroy_source (event_id);
    }
    event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 1000, GSourceFunc(on_timeout), gpointer(this), NULL);
  }
}


gboolean WindowShowRelatedVerses::on_html_link_clicked(GtkHTML * html, const gchar * url, gpointer user_data)
{
  ((WindowShowRelatedVerses *) user_data)->html_link_clicked(url);
  return true;
}


void WindowShowRelatedVerses::html_link_clicked (const gchar * url)
// Callback for clicking a link.
{
  // Store scrolling position for the now active url.
  GtkAdjustment * adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolledwindow));
  scrolling_position[active_url] = gtk_adjustment_get_value (adjustment);

  // New url.
  active_url = url;

  // Start writing a html page.
  HtmlWriter2 htmlwriter ("");
  bool display_another_page = true;

  if (active_url.find ("keyterm ") == 0) {
    item_type = ritKeytermId;
    item_id = active_url.substr (8, 100);
    gtk_button_clicked (GTK_BUTTON (button_item));
    display_another_page = false;
  }

  else if (active_url.find ("strong ") == 0) {
    item_type = ritStrongNumber;
    item_id = active_url.substr (7, 100);
    gtk_button_clicked (GTK_BUTTON (button_item));
    display_another_page = false;
  }

  else if (active_url.find ("parallels") == 0) {
    item_type = ritParallels;
    gtk_button_clicked (GTK_BUTTON (button_item));
    display_another_page = false;
  }

  else {

    // Display the keyterms in the verse, and their renderings.
    htmlwriter.heading_open (3);
    htmlwriter.text_add ("Keyterms in verse " + myreference.verse);
    htmlwriter.heading_close ();
    vector <int> keyterms = keyterms_get_terms_in_verse(myreference);
    for (unsigned int i = 0; i < keyterms.size(); i++) {

      htmlwriter.paragraph_open();

      // Display the keyterm.
      ustring term;
      keyterms_get_term(keyterms[i], term);
      htmlwriter.text_add ("* ");
      ustring url = "keyterm " + convert_to_string (keyterms[i]);
      htmlwriter.hyperlink_add (url, term);
      htmlwriter.text_add (": ");

      // Display the renderings.
      vector <ustring> renderings;
      vector <bool> wholewords;
      vector <bool> casesensitives;
      ustring category;
      {
        ustring dummy1;
        vector < Reference > dummy2;
        keyterms_get_data(keyterms[i], category, dummy1, dummy2);
      }
      keyterms_retrieve_renderings(myproject, term, category, renderings, wholewords, casesensitives);
      for (unsigned int i2 = 0; i2 < renderings.size(); i2++) {
        if (i2)
          htmlwriter.text_add (", ");
        htmlwriter.text_add (renderings[i2]);
      }

      htmlwriter.paragraph_close();
    }
    
    // If there are no keyterms, mention this.
    if (keyterms.empty()) {
      htmlwriter.paragraph_open ();
      htmlwriter.text_add ("none");
      htmlwriter.paragraph_close ();
    }

    // Terms derived from the Strong's numbers in this verse.
    htmlwriter.heading_open (3);
    htmlwriter.text_add ("Similar words in other verses");
    htmlwriter.heading_close ();

    // Get the data.
    vector <ustring> strongs;
    vector <ustring> words;
    kjv_get_strongs_data (myreference, strongs, words);

    // Display the data.
    for (unsigned int i = 0; i < strongs.size(); i++) {
      htmlwriter.paragraph_open ();
      htmlwriter.text_add (words[i]);
      htmlwriter.text_add (" ");
      ustring url = "strong " + strongs[i];
      htmlwriter.hyperlink_add (url, strongs[i]);
      htmlwriter.paragraph_close ();
    }
    
    // Parallel passages.
    vector <Reference> parallel_references;
    vector <ustring> parallel_comments;
    parallel_passages_retrieve (myreference, parallel_references, parallel_comments);
    if (!parallel_references.empty()) {
      extern Settings *settings;
      ProjectConfiguration *projectconfig = settings->projectconfig(myproject);
      ustring language = projectconfig->language_get();
      htmlwriter.heading_open (3);
      htmlwriter.text_add ("Parallel passages");
      htmlwriter.heading_close ();
      for (unsigned int i = 0; i < parallel_references.size(); i++) {
        htmlwriter.paragraph_open ();
        htmlwriter.text_add (parallel_references[i].human_readable (language) + " " + parallel_comments[i]);
        htmlwriter.paragraph_close ();
      }
      htmlwriter.paragraph_open ();
      htmlwriter.hyperlink_add ("parallels", "Send to references window");
      htmlwriter.paragraph_close ();
    }    

  }
  
  htmlwriter.finish();
  if (display_another_page) {
    // Load the page.
    GtkHTMLStream *stream = gtk_html_begin(GTK_HTML(htmlview));
    gtk_html_write(GTK_HTML(htmlview), stream, htmlwriter.html.c_str(), -1);
    gtk_html_end(GTK_HTML(htmlview), stream, GTK_HTML_STREAM_OK);
    // Scroll to the position that possibly was stored while this url was last active.
    gtk_adjustment_set_value (adjustment, scrolling_position[active_url]);
  }
}


void WindowShowRelatedVerses::thread_start(gpointer data)
{
  ((WindowShowRelatedVerses *) data)->thread_main(data);
}


void WindowShowRelatedVerses::thread_main(gpointer data)
{
  thread_runs = false;
}


bool WindowShowRelatedVerses::on_timeout(gpointer user_data)
{
  return ((WindowShowRelatedVerses *) user_data)->timeout();
}


bool WindowShowRelatedVerses::timeout()
{
  html_link_clicked ("");
  //g_thread_create(GThreadFunc(thread_start), gpointer(this), false, NULL); // Todo compiler warning.
  return false;
}


