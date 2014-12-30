<?php
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


Bootstrap::getInstance ();


function page_access_level ($level)
{
  $session_logic = Session_Logic::getInstance ();
  if ($level > $session_logic->currentLevel ()) {
    $header = new Assets_Header ("Privileges");
    $header->setLogin ();
    $header->run ();
    $view = new Assets_View (__FILE__);
    $view->render ("privileges.php");
    Assets_Page::footer ();
    die ();
  }
}


class Bootstrap
{
  private static $instance;
  public $bibledit_root_folder;


  private function __construct()
  {

    // Client mode setup.
    // In case the client mode is prepared, but not enabled,
    // the bootstrap script forwards to the client mode setup page,
    // unless it is already going to that page.
    if (config_logic_client_prepared ()) {
      if (!config_logic_client_enabled ()) {
        @$uri = $_SERVER ["REQUEST_URI"];
        $path = parse_url ($uri, PHP_URL_PATH);
        $folder = pathinfo ($path, PATHINFO_DIRNAME);
        $folder = basename ($folder);
        $page = pathinfo ($path, PATHINFO_BASENAME);
        if ($folder != "setup") {
          if ($page != "topbar.php") {
            if ($page != "client.php") {
              Filter_Url::redirect ("../administration/client.php");
              die;
            }
          }
        }
      }
    }
  }
}


?>
