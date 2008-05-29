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


#include "bibledit-help.h"
#include <string>
#include <iostream>


void *myrealloc(void *ptr, size_t size)
{
  // There might be a realloc() out there that doesn't like reallocing NULL 
  // pointers, so we take care of it here.
  if (ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}


size_t WriteMemoryCallback (void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}


static gboolean on_html_url_requested (GtkHTML *html, const gchar *url, GtkHTMLStream *handle, gpointer user_data)
{
  struct MemoryStruct chunk;    
  chunk.memory=NULL; 
  chunk.size = 0;    
    
  //curl_easy_setopt (curl, CURLOPT_VERBOSE, true);
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt (curl, CURLOPT_USERAGENT, "bibledit/1.0");

  curl_easy_perform (curl);

  if (chunk.memory) {
    gtk_html_write (html, handle, chunk.memory, chunk.size);
    g_free (chunk.memory);
  }
  
  gtk_html_end (html, handle, GTK_HTML_STREAM_OK);

  return true;
}


static gboolean on_html_link_clicked (GtkHTML *html, const gchar * url, gpointer user_data)
{
  // Erase "http://" part of the url.
  string myurl (url);
  myurl.erase (0, 7);

  // See what type of link we've got.
  bool internal_link = false;
  bool difficult_link = false;
  if (myurl.length () > 17) {
    if (myurl.find ("51516") != string::npos) {
      string url2 (myurl);
      url2.erase (0, 16);
      if (url2.substr (0, 1) == "#") {
        internal_link = true;
        myurl = url2;
        myurl.erase (0, 1);
      }
    } else {
      difficult_link = true;
    }
    if (myurl.find ("index.html") != string::npos) {
      difficult_link = true;
    }
  }

  // Handle internal link and bail out.
  if (internal_link) {
    gtk_html_jump_to_anchor (html, myurl.c_str());
    return true;
  }
  
  // Set the base to work against.  
  gtk_html_set_base (html, url);

  // Create a new html stream.
  GtkHTMLStream *stream;
  stream = gtk_html_begin (html);
  
  // Initialize memory for storing the page contents.
  struct MemoryStruct chunk;    
  chunk.memory=NULL; 
  chunk.size = 0;
    
  // Make libcurl verbose.
  //curl_easy_setopt (curl, CURLOPT_VERBOSE, true);

  // Specify URL to get.
  curl_easy_setopt (curl, CURLOPT_URL, url);
  
  // Send all data to this function.
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  
  // We pass our 'chunk' struct to the callback function.
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *)&chunk);
  
  // Some servers don't like requests that are made without a user-agent field, so we provide one.
  curl_easy_setopt (curl, CURLOPT_USERAGENT, "bibledit/1.0");

  if (!difficult_link) {

    // Get it!
    curl_easy_perform (curl);

    // Write the contents to the widget, and free the data.
    if (chunk.memory) {
      gtk_html_write (html, stream, chunk.memory, chunk.size);
      g_free (chunk.memory);
    }
    
  }
  
  // Finish the stream okay.
  gtk_html_end (html, stream, GTK_HTML_STREAM_OK);

  // A difficult link quits the browser.
  if (difficult_link) {
    myurl = "http://localhost:51516/_difficult_url_";
    myurl.append (url);
    curl_easy_setopt (curl, CURLOPT_URL, myurl.c_str());
    curl_easy_perform (curl);
    gtk_main_quit ();
  }
  
  // Done.
  return true;
}


int main (int argc, char *argv[])
{
  // Do not run as root.
  if (getuid () == 0)
    return 1;

  // Initialize bits.
  gtk_init (&argc, &argv);
  curl_global_init (CURL_GLOBAL_ALL);
  curl = curl_easy_init ();

  // URL to load.
  gchar * url = NULL;
  if (argc > 1) url = argv[1];

  // Size and position.
  gint width = 640;
  if (argc > 2) width = strtol (argv[2], &argv[2], 0);
  gint height = 480;
  if (argc > 3) height = strtol (argv[3], &argv[3], 0);
  gint x = 0;
  if (argc > 4) x = strtol (argv[4], &argv[4], 0);
  gint y = 0;
  if (argc > 5) y = strtol (argv[5], &argv[5], 0);
  
  // Display the helpwindow.      
  helpwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (helpwindow), "Help");
  gtk_window_set_default_size (GTK_WINDOW (helpwindow), width, height);
  gtk_widget_show (helpwindow);
  gtk_window_move (GTK_WINDOW (helpwindow), x, y);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER (helpwindow), scrolledwindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  htmlview = gtk_html_new ();
  gtk_widget_show (htmlview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), htmlview);
  
  // Proceed with html widget.
  gtk_html_set_editable (GTK_HTML (htmlview), FALSE);
  gtk_html_allow_selection (GTK_HTML (htmlview), TRUE);
  g_signal_connect (G_OBJECT (htmlview), "url-requested", G_CALLBACK (on_html_url_requested), NULL);
  g_signal_connect (G_OBJECT (htmlview), "link-clicked", G_CALLBACK (on_html_link_clicked), NULL);    

  // Optionally load the url.
  if (url) {
    on_html_link_clicked (GTK_HTML (htmlview), url, NULL);
  }
  
  // Connect signal handlers.
  g_signal_connect ((gpointer) helpwindow, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

  // Main loop.  
  gtk_main ();

  // Done.
  if (curl) curl_easy_cleanup (curl);
  return 0;
}
