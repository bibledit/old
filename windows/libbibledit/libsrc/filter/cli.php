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


class Filter_Cli
{


  // If the script is called from the cli server, this filter does nothing.
  // If called from the web server, the filter dies.
  public static function assert ()
  {
    if (php_sapi_name () != "cli") die;
  }


  // If the argument exists on the command line, it returns that.
  // Or if it exists in the GET variable, it returns that.
  public static function argument ($argv, $key)
  {
    $argument = "";
    if (isset ($argv [$key])) $argument = $argv [$key];
    if (isset ($_GET [$key])) $argument = $_GET [$key];
    return $argument;
  }
  
  
  // The function returns true if Bibledit is not yet ready to run scripts.
  public static function not_yet_ready ()
  {
    // While setup is not yet complete, it is not yet ready.
    $path = dirname (__DIR__) . "../setup";
    if (file_exists ($path)) return true;
    
    // When client mode is prepared, but not yet enabled, it is not yet ready.
    if (Filter_Client::prepared ()) {
      if (!Filter_Client::enabled ()) {
        return true;
      }
    }
    
    // Ready to run.
    return false;
  }


}


?>
