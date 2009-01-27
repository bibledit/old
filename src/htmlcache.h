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

#ifndef INCLUDED_HTML_CACHE_H
#define INCLUDED_HTML_CACHE_H

#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "reference.h"
#include "types.h"
extern "C" {
#include <gtkhtml/gtkhtml.h>
}
#include <curl/curl.h>

struct CurlMemoryStruct
{
  char *memory;
  size_t size;
};

class HtmlCache
{
public:
  HtmlCache(int dummy);
  ~HtmlCache();
  char * request_url(ustring url, size_t& size, bool& trylater);
private:
  ustring clean_url(ustring url);
  ustring cache_name(ustring url);
  ustring cache_error(ustring url);

  vector <ustring> url_queue;
  static void thread_start(gpointer data);
  void thread_main();
  bool abort_thread;
  unsigned int thread_runs;

  static void *myrealloc(void *ptr, size_t size);
  static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

  static int on_progress_function(gpointer user_data, double t, double d, double ultotal, double ulnow);
  int progress_function ();
};

#endif
