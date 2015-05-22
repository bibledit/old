<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


class Filter_Demo
{


  // Returns true for username/password 'admin' in an open demo installation.
  // Else returns false.
  public static function open_demo ($user, $pass)
  {
    include ("config/open.php");
    if ($open_installation) {
      if ($user == "admin") {
        if (($pass == 'admin') || ($pass == md5 ("admin"))) {
          return true;
        }
      }
    }
  }


  // Returns the address of the current demo server.
  public static function demo_address ()
  {
    return "http://bibleconsultants.nl/bibledit-web-demo";
  }
  

  // Returns a warning in case the client is connected to the open demo server.
  public static function client_demo_warning ()
  {
    $warning = "";
    if (Filter_Client::enabled ()) {
      $database_config_general = Database_Config_General::getInstance ();
      $address = $database_config_general->getServerAddress ();
      if ($address == self::demo_address ()) {
        $warning = Locale_Translate::_("Warning:") 
           . " " . Locale_Translate::_("The client is connected to a public demo server.") 
           . " " . Locale_Translate::_("Everybody can modify the data on that server.")
           . " " . Locale_Translate::_("After send and receive your data will reflect the data on the server.");
      }
    }
    return $warning;
  }


}

?>
