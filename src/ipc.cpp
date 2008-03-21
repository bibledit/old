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


#include "libraries.h"
#include "ipc.h"
#include <glib.h>


InterprocessCommunication::InterprocessCommunication (IPCSocketType name)
{
  // Initialize variables.
  listener_running = false;
  method_called_signal = NULL;
  myname = name;

  // Create a socket.
  if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
    log ("No socket", true);
    return;
  }

  // Remove any preexisting socket (or other file)
  unlink (socketname (name).c_str());

  // Unix domain socket
  address.sun_family = AF_UNIX;	
  strcpy(address.sun_path, socketname (name).c_str());

  // The total length of the address includes the sun_family element
  addrLength = sizeof(address.sun_family) + strlen(address.sun_path);

  // Bind to the socket.  
  if (bind(sock, (struct sockaddr *) &address, addrLength)) {
    log ("Cannot bind to socket", true);
    return;
  }

  // Listen to the socket.
  if (listen(sock, 10)) {
    log ("Cannot listen to socket", true);
    return;
  }
  
  // Start the thread with the connection loop.
  listener_run = true;
  g_thread_create (GThreadFunc (listener_start), gpointer (this), false, NULL);
}


InterprocessCommunication::~InterprocessCommunication ()
{
  // Indicate to the thread that we want to stop.
  listener_run = false;
  
  // Connect to our socket and close, so as to exit the waiting loop.
  // If this is going to give problems of bibledit not shutting down, 
  // consider using a non-blocking socket.
  vector <ustring> dummy;
  send (myname, ipcctEnd, dummy);
  
  // Wait until the thread has exited.
  while (listener_running)
    g_usleep (10000);
  
  // Close the socket.
  if (sock >= 0) {
    close(sock);
  }    

  // Remove the socket (or other file)
  unlink (socketname (myname).c_str());
  
  // Destroy the signal button.
  if (method_called_signal) gtk_widget_destroy (method_called_signal);
}


ustring InterprocessCommunication::socketname (IPCSocketType socket)
{
  ustring filename;
  gchar * name = NULL;
  switch (socket) {
    case ipcstBibleditBin:
    {
      name = g_build_filename (g_get_home_dir (), ".bibledit.bin.socket", NULL);
      break;
    }
    case ipcstBibleditBibletime:
    {
      name = g_build_filename (g_get_home_dir (), ".bibledit.bibletime.socket", NULL);
      break;
    }
    case ipcstBibleditGit: 
    {
      name = g_build_filename (g_get_home_dir (), ".bibledit.git.socket", NULL);
      break;
    }
  }
  filename = name;
  g_free (name);
  return filename;
}


void InterprocessCommunication::listener_start (gpointer data)
{
  ((InterprocessCommunication*) data)->listener_main();
}


void InterprocessCommunication::listener_main ()
// Listens for connections and handles them.
{
  // Indicate that the thread runs.
  listener_running = true;
  // While the flag is set loop listening for connections.
  while (listener_run) {
    // Buffer holding the data.
    vector <ustring> message;
    // Wait for a connection.
    if ((conn = accept (sock, (struct sockaddr *) &address, &addrLength)) >= 0) {
      // Read the message sent.
      ustring buffer;
      char buf[1024];
      int amount;
      while ((amount = read (conn, buf, sizeof (buf))) > 0) {
        buf[amount] = '\0';
        buffer.append (buf);
        size_t newlineposition = buffer.find ("\n");
        while ((newlineposition = buffer.find ("\n")) != string::npos) {
          ustring line = buffer.substr (0, newlineposition);
          message.push_back (line);
          buffer.erase (0, newlineposition + 1);
        }
      }
      if (!buffer.empty()) {
        message.push_back (buffer);
      }
      if (amount < 0) {
        log ("Read error", false);
      }
      close (conn);
    }
    if (conn < 0) {
      log ("Error accepting connection", true);
    }
    if (!message.empty()) {
      // Store the message.
      method_called_type = ipcctEnd;   
      istringstream r (message[0]);
      int method;
      r >> method;
      method_called_type = (IPCCallType)method;
      message.erase (message.begin());
      if (method_called_type != ipcctEnd) {
        methodcalls[method_called_type] = message;
      }
      // See whether to emit a signal on this method.
      if (signalling_methods.find (method_called_type) != signalling_methods.end()) {
        gtk_button_clicked (GTK_BUTTON (method_called_signal));
      }
    }
  }
  // Indicate that the thread quitted.
  listener_running = false;
}


void InterprocessCommunication::log (const ustring& message, bool critical)
{
  ustring msg = "InterprocessCommunication: " + message;
  if (critical) {
    g_critical ("%s", message.c_str());
  } else {
    write (1, message.c_str(), strlen (message.c_str()));
    write (1, "\n", 1);
  }
}


bool InterprocessCommunication::send (IPCSocketType name, IPCCallType method, const vector<ustring>& payload)
// Sends "method" and "payload" to the destination socket "name".
{
  // Connect to the socket given in "name".
  struct sockaddr_un address;
  int sock;
  size_t addrLength;
  if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
    log ("Sending: socket error", true);
    return false;
  }
  address.sun_family = AF_UNIX;
  strcpy(address.sun_path, socketname (name).c_str());
  addrLength = sizeof(address.sun_family) + strlen(address.sun_path);
  if (connect(sock, (struct sockaddr *) &address, addrLength)) {
    log ("Sending: connect error", true);
    return false;
  }
  
  // Send the call type.
  bool success = true;
  ostringstream r;
  r << (int) method;
  ustring calltype (r.str());
  if (write (sock, calltype.c_str(), strlen (calltype.c_str())) < 0) {
    log ("Writing error", true);
    success = false;
  }
  write (sock, "\n", 1);

  // Send the data.
  if (success) {
    for (unsigned int i = 0; i < payload.size(); i++) {
      write (sock, payload[i].c_str(), strlen (payload[i].c_str()));
      write (sock, "\n", 1);
    }
  }
  
  // Close the socket.
  close (sock);
  
  // Return success.
  return success;
}


vector <ustring> InterprocessCommunication::receive (IPCSocketType name, IPCCallType method, const vector<ustring>& payload, int timeout)
// Sends "method" and "payload" to the destination socket "name".
// Wait for the reply "method" to come back.
// Don't wait more than "timeout" seconds.
{
  // Initialize variables.
  method_called_type = ipcctEnd;
  time_t oldtime = time (0);
  erase_payload (method);
  
  // Send the payload off.
  bool success = send (name, method, payload);
  
  // Wait for the reply to come back.
  if (success) {
    while ((method_called_type != method) && (time (0) <= (oldtime + timeout))) {
      g_usleep (10);
    }
  }

  // Get the answer, whether it was there or not.  
  vector <ustring> answer = get_payload (method);
  return answer;
}


vector<ustring> InterprocessCommunication::get_payload (IPCCallType method)
{
  vector<ustring> payload;
  payload = methodcalls[method];
  return payload;
}


void InterprocessCommunication::erase_payload (IPCCallType method)
{
  methodcalls[method].clear();
}


void InterprocessCommunication::methodcall_add_signal (IPCCallType method)
// Add a signal to the method call "method".
{
  // Create the signalling button on adding the first method.
  if (!method_called_signal) {
    method_called_signal = gtk_button_new ();
  }
    
  // Store this method.
  signalling_methods.insert (method);
}


void InterprocessCommunication::methodcall_remove_all_signals ()
// Remove all signals from the methodcalls.
{
  signalling_methods.clear();
}


/*
If the script runs on its own, how will the ipc thread crash it?
IPC won't start in scripting now. How will scripting get the values then?
Should be done only through commandline arguments.

*/

// Sometimes the gui hangs for a short while.
// Send on ipc using a thread. 
// This requires a second thread doing the sending, 
// in addition to the one already there for receiving.


// todo for security and robustness, use TCP/IP sockets only for Windows,
// usign the #ifdef WIN32 directive.

/*
Please do be aware that there is an inherent security issue 
with TCP/IP sockets that isn't there for unix ones. Keeping BE from
becoming a security risk wouldn't be a nice task.

Does windows have (and obey!) the ability to restrict the listener to
localhost? If not there could be nasty problems!

Also, I've seen linux boxes without lo enabled,  (and it's easy enough
to turn it off on a running box, using ifconfig). If (private) interprocess
comm is needed, then I'd stick with unix domain sockets where available.

Since it seems that TCP/IP is desirable in some situations, 
then I think I'd recommend a configuration option to allow it but default
to using unix domain sockets, rather than just switching.
*/
