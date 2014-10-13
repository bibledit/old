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


class Filter_Client
{


  // Returns whether Client mode is prepared during setup.
  public static function prepared ()
  {
    // The configuration file may be present or be absent.
    @include ("config/client.php");
    if (isset ($client_mode)) return $client_mode;
    return false;
  }


  // Returns whether Client mode is enabled.
  public static function enabled ()
  {
    $database_config_general = Database_Config_General::getInstance ();
    $enabled = $database_config_general->getClientMode ();
    if ($enabled) return true;
    return false;
  }


  // Sets the Client mode.
  // $enable: boolean: true or false.
  public static function set ($enable)
  {
    $database_config_general = Database_Config_General::getInstance ();
    $database_config_general->setClientMode ($enable);
  }
  

  // This function does the initial connection from the client to the server.
  // It receives settings from the server and applies them to the client.
  // It returns the level of the user.
  // It returns false in case of failure.
  public static function setup ($user = "", $hash = "")
  {
    $database_config_general = Database_Config_General::getInstance ();
    $database_users = Database_Users::getInstance ();
    
    if ($user == "") {
      $users = $database_users->getUsers ();
      if (empty ($users)) return false;
      $user = $users [0];
      $hash = $database_users->getmd5 ($user);
    }

    $encoded_user = bin2hex ($user);

    $address = $database_config_general->getServerAddress ();

    $url = "$address/sync/setup.php?user=$encoded_user&pass=$hash";

    @$response = file_get_contents ($url);
    if (($response >= Filter_Roles::GUEST_LEVEL) && ($response <= Filter_Roles::ADMIN_LEVEL)) {
      // Set user's role on the client to be the same as on the server.
      $database_users->updateUserLevel ($user, $response);
    }

    return $response;
  }
  

  public static function createNoteEncode ($bible, $book, $chapter, $verse, $summary, $contents, $raw)
  {
    $data = array ();
    $data [] = $bible;
    $data [] = $book;
    $data [] = $chapter;
    $data [] = $verse;
    $data [] = $summary;
    $data [] = $raw;
    $data [] = $contents;
    $data = implode ("\n", $data);
    return $data;
  }


  public static function createNoteDecode ($data)
  {
    $data = explode ("\n", $data);
    $result = array ();
    $result ["bible"] = array_shift ($data);
    $result ["book"] = array_shift ($data);
    $result ["chapter"] = array_shift ($data);
    $result ["verse"] = array_shift ($data);
    $result ["summary"] = array_shift ($data);
    $result ["raw"] = array_shift ($data);
    $result ["contents"] = implode ("\n", $data);
    return $result;
  }


}


?>
