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

To call a function, it is put into the object.
A GtkButton is created, which is made available.
This button is destroyed when the function is ready, but it is clicked first, so that any caller knows tht something is ready.

*/
