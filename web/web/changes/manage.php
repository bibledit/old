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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MANAGER_LEVEL);
Assets_Page::header (Locale_Translate::_("Changes"));
$view = new Assets_View (__FILE__);


$database_modifications = Database_Modifications::getInstance ();
$config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();


@$clear = $_GET['clear'];
if (isset ($clear)) {
  // Just in case there are many change notifications to clear, be sure the script does not time out or abort.
  ignore_user_abort (true);
  set_time_limit (0);
  $database_modifications->clearNotificationsUser ($clear);
}


$users = Access_User::assignees ();
$pendingUsers = array ();
$pendingCount = array ();
foreach ($users as $user) {
  $ids = $database_modifications->getNotificationIdentifiers ($user);
  $count = count ($ids);
  if ($count > 0) {
    $pendingUsers [] = $user;
    $pendingCount [] = $count;
  }
}


$view->view->users = $pendingUsers;
$view->view->count = $pendingCount;


$view->render ("manage.php");
Assets_Page::footer ();


?>
