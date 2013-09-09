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
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Changes"));
$view = new Assets_View (__FILE__);


$database_users = Database_Users::getInstance ();
$database_changes = Database_Changes::getInstance ();
$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();


if (isset($_GET['generate'])) {
  $config_general->setTimerDiff (time ());
  $view->view->success = gettext ("The lists of changes will be generated within a minute.");
  $database_logs->log ("The lists of changes will be generated within a minute");
}


@$clear = $_GET['clear'];
if (isset ($clear)) {
  $database_changes->clearUser ($clear);
  $view->view->success = gettext ("The change notifications were cleared for the user.");
}


$users = $database_users->getUsers ();
$pendingUsers = array ();
$pendingCount = array ();
foreach ($users as $user) {
  $ids = $database_changes->getIdentifiers ($user);
  $count = count ($ids);
  if ($count > 0) {
    $pendingUsers [] = $user;
    $pendingCount [] = $count;
  }
}


$view->view->users = $pendingUsers;
$view->view->count = $pendingCount;


$view->render ("changes.php");
Assets_Page::footer ();


?>
