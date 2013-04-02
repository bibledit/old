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


#include "c_url.h"
#include "gwrappers.h"
#include "tiny_utilities.h"


struct CurlMemoryStruct
{
  char *memory;
  size_t size;
};


void *curl_realloc(void *ptr, size_t size)
// There might be a realloc() out there that doesn't like reallocing NULL 
// pointers, so we take care of it here.
{
  if (ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}


size_t curl_write_memory_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  struct CurlMemoryStruct *mem = (struct CurlMemoryStruct *)data;
  mem->memory = (char *)curl_realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}


int on_progress_function(gpointer user_data, double t, double d, double ultotal, double ulnow)
// t: download total, d: download now, ultotal/ulnow: uploads.
/*
 Function pointer that should match the curl_progress_callback prototype found in <curl/curl.h>. 
 This function gets called by libcurl instead of its internal equivalent with a frequent interval during operation 
 (roughly once per second) no matter if data is being transfered or not. 
 Unknown/unused argument values passed to the callback will be set to zero (like if you only download data, 
 the upload size will remain 0). 
 Returning a non-zero value from this callback will cause libcurl to abort the transfer and return CURLE_ABORTED_BY_CALLBACK.
 If you transfer data with the multi interface, this function will not be called during periods of idleness unless you call the 
 appropriate libcurl function that performs transfers. 
 Usage of the CURLOPT_PROGRESSFUNCTION callback is not recommended when using the multi interface.
 CURLOPT_NOPROGRESS must be set to FALSE to make this function actually get called.   
 */
{
  //return ((HtmlCache *) user_data)->progress_function();
  return 0;
}


void curl_http_server_access ()
{
/*

  // Get the url for testing the local server.
  ustring url = "localhost";
  
  // Initialize memory for storing the page contents.
  struct CurlMemoryStruct chunk;
  chunk.memory = NULL;
  chunk.size = 0;

  // Curl pointer.
  extern CURL *curl;

  // Make libcurl verbose.
  //curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

  // Specify URL to get.
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

  // Send all data to this function.
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_memory_callback);

  // We pass our 'chunk' struct to the callback function.
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  // Some servers don't like requests that are made without a user-agent field, so we provide one.
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "bibledit/1.0");

  // Follow any Location: header that the server sends as part of an HTTP header.
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);

  // Automatically set the Referer: field in requests where it follows a Location: redirect. 
  curl_easy_setopt(curl, CURLOPT_AUTOREFERER, true);

  // Set callback function for progress information. Also used for aborting the transfer.
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
  curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, on_progress_function);
  //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

  // Error buffer.
  char curl_errbuf[CURL_ERROR_SIZE];
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);

  // Get it!
  CURLcode code = curl_easy_perform(curl);
 
  // Handle ok or error.
  if (code == 0) {

    // Get and cache the contents.
    if (chunk.memory) {
      
      //g_file_set_contents(cache_name(url).c_str(), chunk.memory, chunk.size, NULL);
      g_free(chunk.memory);
    }

  } else {

    // Error.
    gw_critical (curl_errbuf);
    gw_critical ("Could not connect to the local web server");

  }
  */
}





/*

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


  static int on_progress_function(gpointer user_data, double t, double d, double ultotal, double ulnow);
  int progress_function ();
};






HtmlCache::HtmlCache(int dummy)
{
  thread_runs = 0;
  abort_thread = false;
}

HtmlCache::~HtmlCache()
{
  abort_thread = true;
  while (thread_runs) {
    gw_message("Waiting for html cache to shut down");
    g_usleep(500000);
  }
}

ustring HtmlCache::clean_url(ustring url)
{
  // Remove optional anchor.  
  Parse parse(url, false, "#");
  if (parse.words.size() > 0)
    url = parse.words[0];
  return url;
}

ustring HtmlCache::cache_name(ustring url)
{
  replace_text(url, "/", "_");
  replace_text(url, "\"", "_");
  url = gw_build_filename(directories_get_htmlcache(), url);
  return url;
}

ustring HtmlCache::cache_error(ustring url)
{
  url = cache_name(url);
  url.append("_error_");
  return url;
}

void HtmlCache::thread_start(gpointer data)
{
  ((HtmlCache *) data)->thread_main();
}

void HtmlCache::thread_main()
{

  // Bail out if thread should abort.
  if (abort_thread)
    return;

  // Thread state flag.
  thread_runs++;

  // Create thread if there's another url in the queue.
  if (!url_queue.empty() && !abort_thread) {
    g_thread_create(GThreadFunc(thread_start), gpointer(this), false, NULL); // Todo compiler warning.
  }
  // Thread state flag.
  thread_runs--;
}

char *HtmlCache::request_url(ustring url, size_t & size, bool & trylater)
// Requests a URL from the cache. Return value must be freed by the caller. Value may be NULL.
{
  // Clean url.
  url = clean_url(url);

  // Empty url: bail out.
  if (url.empty()) {
    trylater = false;
    size = 0;
    return g_strdup("");
  }
  // If the url is in the cache, get it from there and bail out.
  ustring cachename = cache_name(url);
  if (g_file_test(cachename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar *content;
    g_file_get_contents(cachename.c_str(), &content, &size, NULL);
    trylater = false;
    return content;
  }
  // If the code reaches this stage, it means that the url was not in the cache.
  // Therefore ensure that the url is in the fetcher queue.
  bool queue = true;
  for (unsigned int i = 0; i < url_queue.size(); i++) {
    if (url == url_queue[i])
      queue = false;
  }
  if (queue)
    url_queue.push_back(url);

  // Optionally create fetcher thread.
  if (!thread_runs) {
    g_thread_create(GThreadFunc(thread_start), gpointer(this), false, NULL); // Todo compiler warning.
  }
  // If an error message is in the cache, get it from there and bail out.
  ustring cacheerror = cache_error(url);
  if (g_file_test(cacheerror.c_str(), G_FILE_TEST_IS_REGULAR)) {
    gchar *content;
    g_file_get_contents(cacheerror.c_str(), &content, &size, NULL);
    trylater = false;
    return content;
  }
  // If the code gets here it means that nothing related to the url was in the cache.
  // Therefore tell the client to retry and give info about loading.
  trylater = true;
  gchar *content = g_strdup("Loading...");
  size = strlen(content);
  return content;
}

*/
