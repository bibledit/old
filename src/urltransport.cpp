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
#include "ipc.h"


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
  // Clear the system for trapping replies.
  messages_awaiting_reply.clear();
  for (unsigned int i = 0; i < buttons_awaiting_reply.size(); i++) {
    gtk_widget_destroy (buttons_awaiting_reply[i]);
  }
  buttons_awaiting_reply.clear();

  // Cancel pending message.
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
  // Trap a reply.
  vector <SoupMessage *> ::iterator message_iter = messages_awaiting_reply.begin();
  vector <GtkWidget *>   ::iterator button_iter =  buttons_awaiting_reply.begin();
  for (unsigned int i = 0; i < messages_awaiting_reply.size(); i++) {
    if (msg == messages_awaiting_reply[i]) {
      // Make information about the reply available to the caller.
      reply_is_ok = SOUP_STATUS_IS_SUCCESSFUL (msg->status_code);
      reply_body = msg->response_body->data;
      // Get signal button.
      GtkWidget * button = buttons_awaiting_reply[i];
      // Remove trap.
      // Note: Do this before signalling the caller, since the caller may modify the trapping system by placing another message in the queue.
      messages_awaiting_reply.erase(message_iter);
      buttons_awaiting_reply.erase (button_iter);
      // Signal caller.      
      gtk_button_clicked (GTK_BUTTON (button));
      // Done.
      break;
    }
    // Next lot.
    message_iter++;
    button_iter++;
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


void URLTransport::send_message (const ustring& url)
// Sends a message off and forgets about it.
{
  SoupMessage * msg = soup_message_new (SOUP_METHOD_GET, url.c_str());
  soup_session_queue_message (session, msg, SoupSessionCallback (on_message_ready_callback), gpointer (this));
}


GtkWidget * URLTransport::send_message_expect_reply (ustring url)
// Sends a message off, and sets up a system for trapping the reply.
{
  // Button to be clicked when the reply gets trapped.
  GtkWidget * button = gtk_button_new ();
  buttons_awaiting_reply.push_back (button);

  // Send off the message.
  SoupMessage * msg = soup_message_new (SOUP_METHOD_GET, url.c_str());
  messages_awaiting_reply.push_back (msg);
  soup_session_queue_message (session, msg, SoupSessionCallback (on_message_ready_callback), gpointer (this));

  // Return the button.
  return button;
}


