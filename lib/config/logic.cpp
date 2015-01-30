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


#include <config/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <database/books.h>
#include <database/config/general.h>
#include <config.h>


// Returns the Bibledit version number.
string config_logic_version ()
{
  return VERSION;
}


// Returns whether client mode is enabled during configure. Todo
bool config_logic_client_prepared ()
{
  return (CLIENT == "yes");
}


// Returns whether Client mode is enabled.
bool config_logic_client_enabled ()
{
  return Database_Config_General::getClientMode ();
}


// Sets the Client mode.
// $enable: boolean: true or false.
void config_logic_set (bool enable) // Todo
{
  Database_Config_General::setClientMode (enable);
}







