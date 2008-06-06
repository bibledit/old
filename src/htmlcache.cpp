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
#include "htmlcache.h"
#include "settings.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "directories.h"

HtmlCache::HtmlCache(int dummy) {
  thread_runs = 0;
}

HtmlCache::~HtmlCache() {
  abort_thread = true;
  g_usleep(20000);
}

ustring HtmlCache::clean_url(ustring url) {
  // Remove optional anchor.  
  Parse parse(url, false, "#");
  if (parse.words.size() > 0)
    url = parse.words[0];
  return url;
}

ustring HtmlCache::cache_name(ustring url) {
  replace_text(url, "/", "_");
  replace_text(url, "\"", "_");
  url = gw_build_filename(directories_get_htmlcache(), url);
  return url;
}

void HtmlCache::thread_start(gpointer data) {
  ((HtmlCache*) data)->thread_main();
}

void HtmlCache::thread_main() {

  // Thread state flags.
  thread_runs++;
  abort_thread = false; // Todo implement, also on shutdown.

  // Get the url to fetch.
  ustring url;
  if (!url_queue.empty()) {
    url = url_queue[0];
    url_queue.erase(url_queue.begin());
  }

  cout << "HtmlCache::thread_main to fetch " << url << endl; // Todo
  
  // Initialize memory for storing the page contents.
  struct CurlMemoryStruct chunk;
  chunk.memory = NULL;
  chunk.size = 0;

  // Curl pointer.
  extern CURL * curl;

  // Make libcurl verbose.
  curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

  // Specify URL to get.
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  // Send all data to this function.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  // We pass our 'chunk' struct to the callback function.
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  // Some servers don't like requests that are made without a user-agent field, so we provide one.
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "bibledit/1.0");

  // Get it!
  curl_easy_perform(curl);

  // Get and cache the contents.
  if (chunk.memory) {
    g_file_set_contents(cache_name(url).c_str(), chunk.memory, -1, NULL);
    g_free(chunk.memory);
  }

  // Optionally create thread for another url in the queue.
  if (!url_queue.empty()) {
    g_thread_create (GThreadFunc (thread_start), gpointer (this), false, NULL);
  }
  
  // Thread state flags.
  thread_runs--;
}

string HtmlCache::request_url(ustring url, bool& trylater) {

  // Content.
  string content;

  // Clean url.
  url = clean_url(url);

  // Empty url: bail out.
  if (url.empty()) {
    trylater = false;
    return "";
  }

  cout << "HtmlCache::request_url " << url << endl; // Todo

  // If the url is in the cache, get it from there and bail out.
  ustring cachename = cache_name(url);
  if (g_file_test(cachename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar * gcontent;
    g_file_get_contents(cachename.c_str(), &gcontent, NULL, NULL);
    if (gcontent) {
      content = gcontent;
      g_free(gcontent);
    }
    trylater = false;
    cout << "Cache hit" << endl; // Todo
    return content;
  }

  // Ensure that the url is in the queue.
  bool queue = true;
  for (unsigned int i = 0; i < url_queue.size(); i++) {
    if (url == url_queue[i])
      queue = false;
  }
  if (queue)
    url_queue.push_back(url);

  // Optionally create fetcher thread.
  if (!thread_runs) {
    g_thread_create (GThreadFunc (thread_start), gpointer (this), false, NULL);
  }

  // Client is to retry.
  trylater = true;
  content = "Loading...";
  return content;
}

void * HtmlCache::myrealloc(void *ptr, size_t size) {
  // There might be a realloc() out there that doesn't like reallocing NULL 
  // pointers, so we take care of it here.
  if (ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t HtmlCache::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
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

/*
 * Todo the html cache.
 * 
 Preferences: Expires after so many days, clear cache on shutdown, and so on.
 
 
 The cache's clients work like this:
 They request the url from the cache. If it is a hit, fine, load whatever you have. If a miss, try again shortly.
 So that if urls are requested through the page, e.g. an image, and it is a miss, then it loads the page as it is.
 It repeats loading the main page till it is a hit, and then the timer stops.
 
 Error messages may be converted through the cache in visible messages, e.g. "File not found", etc.,
 using libcurl's error messages.
 One message could be "Loading...".
 
 Implement the abort function when bibledit shuts down, so that the transfer is really aborted and bibledit does not hang.
 
 */
