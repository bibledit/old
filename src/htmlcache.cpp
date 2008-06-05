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
  event_id = 0;
}

HtmlCache::~HtmlCache() {
  gw_destroy_source(event_id);
}

bool HtmlCache::on_timeout(gpointer user_data) {
  return ((HtmlCache *) user_data)->timeout();
}

bool HtmlCache::timeout() {
  return false;
}

ustring HtmlCache::request_content(ustring url, bool& hit) {

  // Clean the url, removing the optional anchor.  
  Parse parse(url, false, "#");
  if (parse.words.size() > 0)
    url = parse.words[0];

  // Initialize memory for storing the page contents.
  struct CurlMemoryStruct2 chunk;
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

  // Write the contents to the widget, and free the data.
  if (chunk.memory) {
    //gtk_html_write(html, stream, chunk.memory, chunk.size);
    g_free(chunk.memory);
  }

  return "";

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
  struct CurlMemoryStruct2 *mem = (struct CurlMemoryStruct2 *)data;

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
 We need to make a cache object, which stores all on disk.
 When requesting objects from the cache, the "link-clicked" requests also to fetch it if it's not there.
 But the "url requested" does not ask to fetch if it's not there, so that it never blocks.
 Preferences: Expires after so many days, clear cache on shutdown, and so on.t
 
 While saving to disk, replace the "/" with a space, and the "\" too. 
 
 The cache's clients work like this:
 They request the url from the cache. If it is a hit, fine, load whatever you have. If a miss, try again shortly.
 So that if urls are requested through the page, e.g. an image, and it is a miss, then it loads the page as it is.
 It repeats loading the main page till it is a hit, and then the timer stops.
 
 Error messages may be converted through the cache in visible messages, e.g. "File not found", etc.,
 using libcurl's error messages.
 One message could be "Loading...".
 
 */
