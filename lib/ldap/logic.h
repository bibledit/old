/*
 Copyright (©) 2003-2016 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef INCLUDED_LDAP_LOGIC_H
#define INCLUDED_LDAP_LOGIC_H


#include <config/libraries.h>


void ldap_logic_initialize ();
bool ldap_logic_on (bool log = false);
bool ldap_logic_get (string uri,
                     string user, string password,
                     string binddn, string basedn, string scope, string filter,
                     bool & access, string & email);


#endif
