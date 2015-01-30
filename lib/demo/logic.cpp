/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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


#include <demo/logic.h>
#include <filter/md5.h>
#include <config/logic.h>
#include <config.h>
#include <database/config/general.h>
#include <locale/translate.h>


// Returns true for username/password 'admin' in an open demo installation.
// Else returns false.
bool demo_logic_open_demo (string user, string pass)
{
  if (DEMO == "yes") {
    if (user == "admin") {
      if ((pass == "admin") || (pass == md5 ("admin"))) {
        return true;
      }
    }
  }
  return false;
}


// Returns the address of the current demo server.
string demo_logic_demo_address ()
{
  return "http://bibledit.org:9090";
}


// Returns a warning in case the client is connected to the open demo server.
string demo_logic_client_demo_warning ()
{
  string warning;
  if (config_logic_client_enabled ()) {
    string address = Database_Config_General::getServerAddress ();
    if (address == demo_logic_demo_address ()) {
      warning = gettext("Warning:")
      + " " + gettext("The client is connected to a public demo server.")
      + " " + gettext("Everybody can modify the data on that server.")
      + " " + gettext("After send and receive your data will reflect the data on the server.");
    }
  }
  return warning;
}
