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


#include "bibledit-dbus.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


void start_listener ()
{
	SoupMessage * listener_msg;
	listener_msg = soup_message_new (SOUP_METHOD_GET, "http://localhost/bibledit/ipc/xiphos.php");
  soup_session_queue_message (session, listener_msg, SoupSessionCallback (on_listener_ready_callback), NULL);
}

void on_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{


	if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
    printf ("%s\n", msg->response_body->data);
		string body (msg->response_body->data);
		body = trim (body);
		if (body.find ("quit") == 0) {
      g_main_loop_quit (loop);
		}
	} else {
		printf ("message failure: %s\n", msg->reason_phrase);
		g_usleep (1000000);
	}

	g_usleep (100000);
	start_listener ();
	
}


string trim(const string & s)
{
  if (s.length() == 0)
    return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t beg = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  // No non-spaces  
  if (beg == string::npos)
    return "";
  return string(s, beg, end - beg + 1);
}


/*
static void
get_url (const char *url)
{
	const char *name;
	SoupMessage *msg;
	const char *header;

	msg = soup_message_new (method, url);
	soup_message_set_flags (msg, SOUP_MESSAGE_NO_REDIRECT);

	soup_session_send_message (session, msg);

	name = soup_message_get_uri (msg)->path;

	if (debug) {
		SoupMessageHeadersIter iter;
		const char *hname, *value;
		char *path = soup_uri_to_string (soup_message_get_uri (msg), TRUE);

		printf ("%s %s HTTP/1.%d\n\n", method, path,
			soup_message_get_http_version (msg));
		printf ("HTTP/1.%d %d %s\n",
			soup_message_get_http_version (msg),
			msg->status_code, msg->reason_phrase);

		soup_message_headers_iter_init (&iter, msg->response_headers);
		while (soup_message_headers_iter_next (&iter, &hname, &value))
			printf ("%s: %s\r\n", hname, value);
		printf ("\n");
	} else
		printf ("%s: %d %s\n", name, msg->status_code, msg->reason_phrase);

	if (SOUP_STATUS_IS_REDIRECTION (msg->status_code)) {
		header = soup_message_headers_get_one (msg->response_headers,
						       "Location");
		if (header) {
			if (!debug)
				printf ("  -> %s\n", header);
			get_url (header);
		}
	} else if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		fwrite (msg->response_body->data, 1,
			msg->response_body->length, stdout);
	}
}
*/

int main (int argc, char **argv)
{
  g_thread_init(NULL);
	g_type_init ();

	url = argv[1];

	session = soup_session_async_new_with_options (SOUP_SESSION_USER_AGENT, "bibledit-dbus/1.0", NULL);

  start_listener ();


	loop = g_main_loop_new (NULL, TRUE);
  g_main_loop_run (loop);
	//get_url (url);
  // g_main_loop_quit (loop);
	g_main_loop_unref (loop);

	return 0;
}
