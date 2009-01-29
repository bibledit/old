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
#include "ipc.h"
#include <glib.h>

InterprocessCommunication::InterprocessCommunication(IPCSocketType name)
{
  // Initialize variables.
  listener_running = false;
  method_called_signal = NULL;
  myname = name;

  // Create a socket.
#ifdef WIN32
  // Under Win32, you need to initialize the Winsock environment.
  // It seems this constructor is called once per process, so it
  // should be safe to init here and destroy in the destructor.
  WORD wVersionRequested = MAKEWORD(2, 0);
  WSADATA wsaData;
  if (WSAStartup(wVersionRequested, &wsaData)) {
    log("Couldn't call WSAStartup!", true);
    return;
  }
  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) {
    log("Windows Sockets DLL unusable.", true);
    return;
  }
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
#else
  if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
#endif
    log("No socket", true);
    return;
  }
#ifdef WIN32
  address.sin_family = AF_INET;
  WSAHtons(sock, atoi(socketname(name).c_str()), &address.sin_port);
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  addrLength = sizeof(address);
#else
  // Remove any preexisting socket (or other file)
  unlink(socketname(name).c_str());

  // Unix domain socket
  address.sun_family = AF_UNIX;
  strcpy(address.sun_path, socketname(name).c_str());

  // The total length of the address includes the sun_family element
  addrLength = sizeof(address.sun_family) + strlen(address.sun_path);
#endif

  // Bind to the socket.  
#ifdef WIN32
  if (bind(sock, (struct sockaddr *)&address, addrLength) == SOCKET_ERROR) {
#else
  if (bind(sock, (struct sockaddr *)&address, addrLength)) {
#endif
    log("Cannot bind to socket", true);
    return;
  }
  // Listen to the socket.
#ifdef WIN32
  if (listen(sock, 10) == SOCKET_ERROR) {
#else
  if (listen(sock, 10)) {
#endif
    log("Cannot listen to socket", true);
    return;
  }
  // Start the thread with the connection loop.
  listener_run = true;
  g_thread_create(GThreadFunc(listener_start), gpointer(this), false, NULL);
}

InterprocessCommunication::~InterprocessCommunication()
{
  // Indicate to the thread that we want to stop.
  listener_run = false;

  // Connect to our socket and close, so as to exit the waiting loop.
  // If this is going to give problems of bibledit not shutting down, 
  // consider using a non-blocking socket.
  vector < ustring > dummy;
  send(myname, ipcctEnd, dummy);

  // Wait until the thread has exited.
  while (listener_running)
    g_usleep(10000);

  // Close the socket.
  if (sock >= 0) {
#ifdef WIN32
    closesocket(sock);          // sockets aren't files on WIN32
#else
    close(sock);
#endif
  }
#ifdef WIN32
  // Shutdown Windows Sockets
  WSACleanup();
#else
  // Remove the socket (or other file)
  unlink(socketname(myname).c_str());
#endif

  // Destroy the signal button.
  if (method_called_signal)
    gtk_widget_destroy(method_called_signal);
}

ustring InterprocessCommunication::socketname(IPCSocketType socket)
{
  ustring filename;
  gchar *name = NULL;
  switch (socket) {
  case ipcstBibleditBin:
    {
#ifdef WIN32
      // Need to switch to dynamic and authenticated ports. For now, static unauthenticated.
      name = "33487";           // Touch Tone code for EDITR
#else
      name = g_build_filename(g_get_home_dir(), ".bibledit.bin.socket", NULL);
#endif
      break;
    }
  case ipcstBibleditBibletime:
    {
#ifdef WIN32
      name = "24253";           // BIBLE
#else
      name = g_build_filename(g_get_home_dir(), ".bibledit.bibletime.socket", NULL);
#endif
      break;
    }
  case ipcstBibleditGit:
    {
#ifdef WIN32
      name = "23448";           // BEGIT
#else
      name = g_build_filename(g_get_home_dir(), ".bibledit.git.socket", NULL);
#endif
      break;
    }
  }
  filename = name;
  g_free(name);
  return filename;
}

void InterprocessCommunication::listener_start(gpointer data)
{
  ((InterprocessCommunication *) data)->listener_main();
}

void InterprocessCommunication::listener_main()
// Listens for connections and handles them.
{
  // Indicate that the thread runs.
  listener_running = true;
  // While the flag is set loop listening for connections.
  while (listener_run) {
    // Buffer holding the data.
    vector < ustring > message;
    // Wait for a connection.
#ifdef WIN32
    if ((conn = accept(sock, (struct sockaddr *)&address, &addrLength)) != INVALID_SOCKET) {
#else
    if ((conn = accept(sock, (struct sockaddr *)&address, &addrLength)) >= 0) {
#endif
      // Read the message sent.
      ustring buffer;
      char buf[1024];
      int amount;
#ifdef WIN32
      while ((amount = recv(conn, buf, sizeof(buf), 0)) != SOCKET_ERROR) {
#else
      while ((amount = read(conn, buf, sizeof(buf))) > 0) {
#endif
        buf[amount] = '\0';
        buffer.append(buf);
        size_t newlineposition = buffer.find("\n");
        while ((newlineposition = buffer.find("\n")) != string::npos) {
          ustring line = buffer.substr(0, newlineposition);
          message.push_back(line);
          buffer.erase(0, newlineposition + 1);
        }
      }
      if (!buffer.empty()) {
        message.push_back(buffer);
      }
      if (amount < 0) {
        log("Read error", false);
      }
#ifdef WIN32
      closesocket(conn);
#else
      close(conn);
#endif
    }
    if (conn < 0) {
      log("Error accepting connection", true);
    }
    if (!message.empty()) {
      // Store the message.
      method_called_type = ipcctEnd;
      istringstream r(message[0]);
      int method;
      r >> method;
      method_called_type = (IPCCallType) method;
      message.erase(message.begin());
      if (method_called_type != ipcctEnd) {
        methodcalls[method_called_type] = message;
      }
      // See whether to emit a signal on this method.
      if (signalling_methods.find(method_called_type) != signalling_methods.end()) {
        gtk_button_clicked(GTK_BUTTON(method_called_signal));
      }
    }
  }
  // Indicate that the thread quitted.
  listener_running = false;
}

void InterprocessCommunication::log(const ustring & message, bool critical)
{
  ustring msg = "InterprocessCommunication: " + message;
  if (critical) {
    g_critical("%s", message.c_str());
  } else {
    if (write(1, message.c_str(), strlen(message.c_str()))) ;   // Suppress compiler warning.
    if (write(1, "\n", 1)) ;
  }
}

bool InterprocessCommunication::send(IPCSocketType name, IPCCallType method, const vector < ustring > &payload)
// Sends "method" and "payload" to the destination socket "name".
{
  // Connect to the socket given in "name".
#ifdef WIN32
  struct sockaddr_in address;
#else
  struct sockaddr_un address;
#endif
  int sock;
  size_t addrLength;
#ifdef WIN32
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
#else
  if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
#endif
    log("Sending: socket error", true);
    return false;
  }
#ifdef WIN32
  address.sin_family = AF_INET;
  WSAHtons(sock, atoi(socketname(name).c_str()), &address.sin_port);
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  addrLength = sizeof(address);
  if (connect(sock, (struct sockaddr *)&address, addrLength) == SOCKET_ERROR) {
    log("Sending: connect error", true);
    cerr << "WSAGetLastError: " << WSAGetLastError() << endl;
    return false;
  }
#else
  address.sun_family = AF_UNIX;
  strcpy(address.sun_path, socketname(name).c_str());
  addrLength = sizeof(address.sun_family) + strlen(address.sun_path);
  if (connect(sock, (struct sockaddr *)&address, addrLength)) {
    log("Sending: connect error", true);
    return false;
  }
#endif

  // Send the call type.
  bool success = true;
  ostringstream r;
  r << (int)method;
  ustring calltype(r.str());
#ifdef WIN32
  if (::send(sock, calltype.c_str(), strlen(calltype.c_str()), 0) == SOCKET_ERROR) {
#else
  if (write(sock, calltype.c_str(), strlen(calltype.c_str())) < 0) {
#endif
    log("Writing error", true);
    success = false;
  }
#ifdef WIN32
  ::send(sock, "\n", 1, 0);
#else
  if (write(sock, "\n", 1)) ;   // Suppress compiler warning.
#endif

  // Send the data.
  if (success) {
    for (unsigned int i = 0; i < payload.size(); i++) {
#ifdef WIN32
      ::send(sock, payload[i].c_str(), strlen(payload[i].c_str()), 0);
      ::send(sock, "\n", 1, 0);
#else
      if (write(sock, payload[i].c_str(), strlen(payload[i].c_str()))) ;        // Suppress compiler warning.
      if (write(sock, "\n", 1)) ;
#endif
    }
  }
  // Close the socket.
#ifdef WIN32
  closesocket(sock);
#else
  close(sock);
#endif

  // Return success.
  return success;
}

vector < ustring > InterprocessCommunication::receive(IPCSocketType name, IPCCallType method, const vector < ustring > &payload, int timeout)
// Sends "method" and "payload" to the destination socket "name".
// Wait for the reply "method" to come back.
// Don't wait more than "timeout" seconds.
{
  // Initialize variables.
  method_called_type = ipcctEnd;
  time_t oldtime = time(0);
  erase_payload(method);

  // Send the payload off.
  bool success = send(name, method, payload);

  // Wait for the reply to come back.
  if (success) {
    while ((method_called_type != method) && (time(0) <= (oldtime + timeout))) {
      // The time was extended so that less CPU time is consumed.
      g_usleep(10000);
    }
  }
  // Get the answer, whether it was there or not.  
  vector < ustring > answer = get_payload(method);
  return answer;
}

vector < ustring > InterprocessCommunication::get_payload(IPCCallType method)
{
  vector < ustring > payload;
  payload = methodcalls[method];
  return payload;
}

void InterprocessCommunication::erase_payload(IPCCallType method)
{
  methodcalls[method].clear();
}

void InterprocessCommunication::methodcall_add_signal(IPCCallType method)
// Add a signal to the method call "method".
{
  // Create the signalling button on adding the first method.
  if (!method_called_signal) {
    method_called_signal = gtk_button_new();
  }
  // Store this method.
  signalling_methods.insert(method);
}

void InterprocessCommunication::methodcall_remove_all_signals()
// Remove all signals from the methodcalls.
{
  signalling_methods.clear();
}

/*
 There is an inherent security issue with TCP/IP sockets that isn't there for UNIX ones. 

 Linux boxes without lo enabled have been seen. And it's easy enough to turn it off on a running box, 
 using ifconfig. For interprocess communication therefore it is recommended 
 to use UNIX domain sockets where available.
 */
