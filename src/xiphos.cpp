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


#include "xiphos.h"
#include "settings.h"
#include <sys/types.h>
#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <sys/un.h>
#endif
#include "d_bus.h"


const gchar * xiphos_dbus_object ()
{
  return "/org/xiphos/remote/ipc";
}


const gchar * xiphos_dbus_interface ()
{
  return "org.xiphos.remote";
}



void xiphos_reference_send (Reference reference)
// Sends a reference to Xiphos: "sword://Genesis 1:1"
{
#ifndef WIN32
  extern Settings * settings;
  if (settings->genconfig.reference_exchange_send_to_xiphos_get()) {
    // Check whether the user does not receive referenes from Xiphos at this moment.
    if (!settings->session.receiving_references || !settings->genconfig.reference_exchange_receive_from_xiphos_get()) {
      // Create the reference parameters.
      ustring parameter = "sword://" + reference.human_readable ("");
      // Send it.
      extern DBus * dbus;
      dbus->send_to_xiphos (xiphos_dbus_object (), xiphos_dbus_interface (), "navigate", parameter);
    }
  }
#endif
}


/*

To checkout xiphos:
svn co https://gnomesword.svn.sourceforge.net/svnroot/gnomesword/trunk xiphos


Bazaar repository with xiphos with ipc - work in progress:
Xiphos before being committed to svn.
bzr branch http://www.chaqar.com/bzr/xiphos-ipc xiphos-ipc
From there on, it's fairly straightforward. Just ./autogen as you
normally would. There is one thing you'll have to do manually:
cd src/gnome2
dbus-binding-tool --prefix=ipc_object --mode=glib-server ipc-interface.xml > "ipc-server-stub.h"
After that, make should 'just work'.
To make the client, do this:
cd src/gnome2
make -f receive_make client
and run with ./client. Due to how this is currently programmed, I
think you'll have to start Xiphos before starting the client. I'll
change the client at some point so that shouldn't be necessary.
So, what does it do? Well, using d-feet or some other dbus explorer,
you can call the function "navigate" with sword://Gen 1:1, and Xiphos
will navigate to that. The client will listen on dbus for Xiphos to
emit a "performSearch" signal. This is emitted every time you do a
search in the sidebar (can be easily extended to advanced search).
When this occurs, the client will print the search term to the
terminal. I have a function in place that will allow the client to
then ask for the search results, but it isn't hooked up yet in Xiphos.

*/


