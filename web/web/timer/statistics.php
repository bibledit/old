<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


require_once ("../bootstrap/bootstrap.php");

$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("statistics: Sending statistics to users"), true);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("statistics: Fatal: This only runs through the cli Server API", true);
  die;
}


$database_config_user = Database_Config_User::getInstance ();
$database_users = Database_Users::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_changes = Database_Changes::getInstance ();


$users = $database_users->getUsers ();
foreach ($users as $user) {
  $subject = "Bibledit " . gettext ("statistics");
  $body = array ();
  if ($database_config_user->getUserPendingChangesNotification ($user)) {
    $ids = $database_changes->getIdentifiers ($user);
    $body [] = gettext ("Number of changes awaiting your approval:") . " " . count ($ids);
  }
  $body = implode ("\n", $body);
  var_dump ($body);
  $database_mail->send ($user, $subject, $body);
}


$database_logs->log (gettext ("statistics: Completed"), true);


?>
