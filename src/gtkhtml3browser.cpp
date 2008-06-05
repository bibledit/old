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
#include "gtkhtml3browser.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "books.h"
#include "resource_utils.h"
#include "resourceviewergui.h"

GtkHtml3Browser::GtkHtml3Browser(GtkWidget * vbox) {
  // Save and initialize variables.
  my_vbox = vbox;
  event_id = 0;
  last_focused_time = 0;

  // The html widget.
  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  htmlview = gtk_html_new();
  gtk_widget_show(htmlview);
  gtk_container_add(GTK_CONTAINER (scrolledwindow), htmlview);

  gtk_html_set_editable(GTK_HTML (htmlview), FALSE);
  gtk_html_allow_selection(GTK_HTML (htmlview), TRUE);

  // Signals.
  g_signal_connect (G_OBJECT (htmlview), "url-requested", G_CALLBACK (on_html_url_requested), gpointer(this));
  g_signal_connect (G_OBJECT (htmlview), "link-clicked", G_CALLBACK (on_html_link_clicked), gpointer(this));
  g_signal_connect (G_OBJECT (htmlview), "grab_focus", G_CALLBACK (on_htmlview_grab_focus), gpointer(this));
}

GtkHtml3Browser::~GtkHtml3Browser() {
  gw_destroy_source(event_id);
  gtk_widget_destroy(scrolledwindow);
}

void GtkHtml3Browser::focus() {
  gtk_widget_grab_focus(htmlview);
}

bool GtkHtml3Browser::focused() {
  return GTK_WIDGET_HAS_FOCUS (htmlview);
}

void GtkHtml3Browser::copy() {
  if (focused())
    gtk_html_copy(GTK_HTML (htmlview));
}

void GtkHtml3Browser::go_to(const ustring& url) {
  html_link_clicked(GTK_HTML (htmlview), url.c_str ());
  /*
   gw_destroy_source (event_id);
   event_id = g_timeout_add_full (G_PRIORITY_DEFAULT, 100, GSourceFunc (on_timeout), gpointer(this), NULL);
   */
}

bool GtkHtml3Browser::on_timeout(gpointer user_data) {
  return ((GtkHtml3Browser *) user_data)->timeout();
}

bool GtkHtml3Browser::timeout() {
  return false;
}

gboolean GtkHtml3Browser::on_html_url_requested(GtkHTML *html, const gchar *url, GtkHTMLStream *handle, gpointer user_data) {
  ((GtkHtml3Browser *) user_data)->html_url_requested(html, url, handle);
  return true;
}

void GtkHtml3Browser::html_url_requested(GtkHTML *html, const gchar *url, GtkHTMLStream *handle) {
  //cout << "html_url_requested: " << url << endl; // Todo

  struct CurlMemoryStruct chunk;
  chunk.memory= NULL;
  chunk.size = 0;

  extern CURL * curl;
  curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "bibledit/1.0");

  curl_easy_perform(curl);

  if (chunk.memory) {
    gtk_html_write(html, handle, chunk.memory, chunk.size);
    g_free(chunk.memory);
  }

  gtk_html_end(html, handle, GTK_HTML_STREAM_OK);
}

gboolean GtkHtml3Browser::on_html_link_clicked(GtkHTML *html, const gchar * url, gpointer user_data) {
  ((GtkHtml3Browser *) user_data)->html_link_clicked(html, url);
  return true;
}

void GtkHtml3Browser::html_link_clicked(GtkHTML *html, const gchar * url) {

  //cout << "html_link_clicked: " << url << endl; // Todo

  // Bail out if the url is empty.
  ustring myurl(url);
  if (myurl.empty())
    return;

  // Optionally let the second browser intercept URLs.
  if (browser2 && !url_filter.empty()) {
    if (myurl.find(url_filter) != string::npos) {
      browser2->go_to(myurl);
      return;
    }
  }

  // Handle a single anchor.
  if (myurl.substr(0, 1) == "#") {
    myurl.erase(0, 1);
    gtk_html_jump_to_anchor(html, myurl.c_str());
    return;
  }

  // Parse the url into the url and the anchor.  
  Parse parse(myurl, false, "#");
  if (parse.words.size() > 0)
    myurl = parse.words[0];
  ustring myanchor;
  if (parse.words.size() > 1)
    myanchor = parse.words[1];

  // When a relative link is given, assemble the full url.
  if (gw_path_get_dirname (myurl).empty() || gw_path_get_dirname(myurl) == ".") {
    myurl = gw_build_filename(gw_path_get_dirname(loaded_url), myurl);
  }

  //cout << "My url becomes " << myurl << endl; // Todo

  // Only load a new url if it differs from the one currently loaded.
  if ((myurl != loaded_url) && !myurl.empty()) {

    // Save url loaded.
    loaded_url = myurl;

    // Create a new html stream.
    GtkHTMLStream *stream;
    stream = gtk_html_begin(html);

    // Initialize memory for storing the page contents.
    struct CurlMemoryStruct chunk;
    chunk.memory = NULL;
    chunk.size = 0;

    // Curl pointer.
    extern CURL * curl;

    // Make libcurl verbose.
    curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

    // Specify URL to get.
    curl_easy_setopt(curl, CURLOPT_URL, myurl.c_str());

    // Send all data to this function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    // We pass our 'chunk' struct to the callback function.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    // Some servers don't like requests that are made without a user-agent field, so we provide one.
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "bibledit/1.0");

    // Get it!
    curl_easy_perform(curl);

    // Write the contents to the widget, and free the data.
    if (chunk.memory) {
      gtk_html_write(html, stream, chunk.memory, chunk.size);
      g_free(chunk.memory);
    }

    // Finish the stream okay.
    gtk_html_end(html, stream, GTK_HTML_STREAM_OK);

  }

  // Optionally jump to an anchor.
  if (!myanchor.empty()) {
    gtk_html_jump_to_anchor(html, myanchor.c_str());
  }
}

void * GtkHtml3Browser::myrealloc(void *ptr, size_t size) {
  // There might be a realloc() out there that doesn't like reallocing NULL 
  // pointers, so we take care of it here.
  if (ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t GtkHtml3Browser::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;
  struct CurlMemoryStruct *mem = (struct CurlMemoryStruct *)data;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}

void GtkHtml3Browser::on_htmlview_grab_focus(GtkWidget *widget, gpointer user_data) {
  ((GtkHtml3Browser *) user_data)->htmlview_grab_focus();
}

void GtkHtml3Browser::htmlview_grab_focus()
// This function records the most recent time that the browser received focus.
{
  last_focused_time = time(0);
}

void GtkHtml3Browser::set_second_browser(const ustring& filter, GtkHtml3Browser * browser)
// Instead of doing frames properly, a second browser is used. This browser should intercept URLs clicked.
{
  url_filter = filter;
  browser2 = browser;
}

/*
 Todo building GtkHtml Browser.

 Make general source that handles the "browser" bits. One object called HtmlBrowser does all the 
 bits, threads, waiting, destruction, and so on. It has a method called "stop", and "load",
 and so on.

 You make the write callback (or progress callback) return an error and
 libcurl will then abort the transfer.
 return -1 to signal error to the library (it will cause it to abort the transfer

The system needs a thread to retrieve URLs, because some URLs can take their time to come through, or
block altogether.
The system that is followed will run in a thread, create a gtkhtml widget, does not show it, but just in the air,
and does all the jobs that it has to do. In the mean time it stores all data so received.
Abort can be done by the callback, if bibledit shuts down.
Once all data has been collected, it is once again loaded, but now in the real view.
All the views callbacks retrieve that data from the store
Data is kept in the store for about 10 minutes or so, then discarded.
While a URL has been clicked, a timer is kept running that regularly looks whether the
thread has finished. Once finished, it calls again the routine and now loads from the cache.

Transfer fetcher to the cache as step one.
Then do the storing through the cache as well.
And finally the intelligence to ask the cache twice for data.


 */

// Todo check why the tarball does not compile and no error is given in the nightly builds.
// Probably because the nightbly builds does not compile from the tarball, only from the repository.

// Todo let the configure.ac check for either gtkhtml-3.8 or gtkhtml-3.12, and take either of them.
