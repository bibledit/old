<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Backup"));

$view = new Assets_View (__FILE__);

$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();

if (isset ($_GET ['create'])) {
  $config_general->setTimerBackup (time ());
  $message = gettext ("Will create a backup within a minute");
  $view->view->success = $message;
  $database_logs->log ($message);
}


include ("database/credentials.php");
$view->view->file = Filter_Backup::file ();
$view->view->folder = Filter_Backup::folder ();
$view->view->database_host = $database_host;
$view->view->database_name = $database_name;
$view->view->database_user = $database_user;
$view->view->database_pass = $database_pass;
$view->render ("backup.php");

Assets_Page::footer ();

?>
