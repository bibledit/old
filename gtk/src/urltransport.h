/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_URL_TRANSPORT_H
#define INCLUDED_URL_TRANSPORT_H


#include "libraries.h"
#include <libsoup/soup.h>
#include <gtk/gtk.h>


class URLTransport
{
public:
  URLTransport(int dummy);
  ~URLTransport();
  void send_message (const ustring& url);
  void send_message_in_sequence (const ustring& url);
  GtkWidget * send_message_expect_reply (ustring url);
  bool reply_is_ok;
  ustring reply_body;
private:
  void log(const ustring & message);
  ustring last_message;
  SoupSession *session;
  SoupMessage * server_test_msg;

  static void on_message_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);
  void on_message_ready (SoupSession *session, SoupMessage *msg);

  static void on_message_in_sequence_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);
  void on_message_in_sequence_ready (SoupSession *session, SoupMessage *msg);
  bool sequential_messages_in_transit;
  deque <ustring> pending_message_urls;
  void send_next_message_in_sequence ();

  static void on_message_with_reply_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);
  void on_message_with_reply_ready (SoupSession *session, SoupMessage *msg);
  vector <SoupMessage *> messages_awaiting_reply;
  vector <GtkWidget *> buttons_awaiting_reply;
};


#endif
