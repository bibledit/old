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
#include "utilities.h"
#include "urltransport.h"
#include <glib/gthread.h>
#include "gwrappers.h"
#include "tiny_utilities.h"
#include <libsoup/soup.h>


URLTransport::URLTransport(int dummy)
// URL transporter.
{
  // Create a session.
  session = soup_session_async_new_with_options (SOUP_SESSION_USER_AGENT, "bibledit/1.0", 
                                                 SOUP_SESSION_MAX_CONNS, "50", 
                                                 SOUP_SESSION_MAX_CONNS_PER_HOST, "50",
                                                 NULL);
  //SoupLogger *logger;
  //logger = soup_logger_new(SOUP_LOGGER_LOG_HEADERS, -1);
  //logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
  //soup_session_add_feature (session, SOUP_SESSION_FEATURE (logger));
  //g_object_unref (logger);
  
  // Initial message to test the server.
  server_test_msg = soup_message_new (SOUP_METHOD_GET, "http://localhost");
  soup_session_queue_message (session, server_test_msg, SoupSessionCallback (on_message_ready_callback), gpointer (this));
}


URLTransport::~URLTransport()
{
  // Cancel any running messages.
  if (server_test_msg) {
    soup_session_cancel_message (session, server_test_msg, SOUP_STATUS_CANCELLED);
  }
  // Just to be sure, abort the session.
  soup_session_abort (session);
}


void URLTransport::on_message_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
  ((URLTransport *) user_data)->on_message_ready(session, msg);

}


void URLTransport::on_message_ready (SoupSession *session, SoupMessage *msg)
{
  // Handle the initial server test message
  if (msg == server_test_msg) {
    if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
      log ("ok");
    } else {
      log (msg->reason_phrase);
    }
    server_test_msg = NULL;
  }
  // Erase the message from the list of active ones.
  vector <SoupMessage *> ::iterator iter;
  iter = active_messages.begin();
  for (unsigned int i = 0; i < active_messages.size(); i++) {
    if (msg == active_messages[i]) {
      active_messages.erase(iter);
      break;
    }
    iter++;
  }
}


void URLTransport::log(const ustring & message)
// Logs messages. 
// It does not output the same message repeatedly.
{
  if (message != last_message) {
    gw_message("Webserver Access: " + message);
    last_message = message;
  }
}


void URLTransport::signal (const ustring& url)
// A signal is a message that is sent, but nobody cares whether it
// managed to get through, and nobody cares about the response.
{
  SoupMessage * msg = soup_message_new (SOUP_METHOD_GET, url.c_str());
  active_messages.push_back (msg);
  soup_session_queue_message (session, msg, SoupSessionCallback (on_message_ready_callback), gpointer (this));
}








/*

Todo web access.


(bibleditgui:20294): libsoup-CRITICAL **: soup_session_queue_message: assertion `SOUP_IS_MESSAGE (msg)' failed
http://localhost/bibledit/ipc/storemessage.php?recipient=xiphos&subject=goto&message=sword://Ezekiel 22:3


To call a function, it is put into the object.
A GtkButton is created, which is made available.
This button is destroyed when the function is ready, but it is clicked first, so that any caller knows tht something is ready.


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
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);

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
