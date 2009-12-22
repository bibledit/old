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



ustring xiphos_reference_create (Reference reference)
// Creates a reference for Xiphos: "sword://Genesis 1:1"
// (For the time being, it will also send it)
{
  ustring parameter;
#ifndef WIN32
  extern Settings * settings;
  if (settings->genconfig.reference_exchange_send_to_xiphos_get()) {
    // Check whether the user does not receive referenes from Xiphos at this moment.
    if (!settings->session.receiving_references || !settings->genconfig.reference_exchange_receive_from_xiphos_get()) {
      // Create the reference parameters.
      parameter = "sword://" + reference.human_readable ("");
      // Send it.
      extern DBus * dbus;
      dbus->send_to_xiphos (xiphos_dbus_object (), xiphos_dbus_interface (), "setCurrentReference", parameter);
    }
  }
#endif
  return parameter;
}


