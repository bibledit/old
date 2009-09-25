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
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <sys/un.h>


/*

To checkut xiphos:
svn co https://gnomesword.svn.sourceforge.net/svnroot/gnomesword/trunk xiphos

*/


void xiphos_reference_send (Reference reference)
// Sends a reference to Xiphos: "sword://Genesis 1:1"
{
  extern Settings * settings;
  if (settings->genconfig.reference_exchange_send_to_xiphos_get()) {
    int read_socket;
    struct sockaddr_un read_serv;
    socklen_t addrlen;
    memset(&read_serv, 0, sizeof(read_serv));
    read_serv.sun_family = AF_LOCAL;
    strcpy(&read_serv.sun_path[1], "/org/xiphos/write");
    addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(read_serv.sun_path + 1) + 1;
    read_socket = socket(AF_LOCAL, SOCK_DGRAM, 0);
    connect(read_socket, (struct sockaddr *) &read_serv, addrlen);
    ustring parameter = "sword://" + reference.human_readable ("");
    if (write(read_socket, parameter.c_str(), strlen(parameter.c_str()) + 1));
    close(read_socket);
  }
}


