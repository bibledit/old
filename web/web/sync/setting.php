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


$username = Filter_Hex::hex2bin ($_POST ['u']);
$password = $_POST ['p'];
$level = $_POST ['l'];
$id = $_POST ['i'];
$setting = unserialize ($_POST ['s']);


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_users = Database_Users::getInstance ();
$session_logic = Session_Logic::getInstance ();


$user_ok = $database_users->usernameExists ($username);
if (!$user_ok) $database_logs->log ("Non existing user $username", Filter_Roles::MANAGER_LEVEL);
$pass_ok = ($password == $database_users->getmd5 ($username));
if (!$pass_ok) $database_logs->log ("Incorrect password $password for user $username", Filter_Roles::MANAGER_LEVEL);
$level_ok = ($level == $database_users->getUserLevel ($username));
if (!$level_ok) $database_logs->log ("Incorrect role $level for user $username", Filter_Roles::MANAGER_LEVEL);
if (!$user_ok || !$pass_ok || !$level_ok) {
  // Unauthorized.
  http_response_code (401); 
  die;
}


$session_logic->setUsername ($username);


if ($id == Sync_Logic::WORKBENCH_SETTING) {

  $urls = $setting ['urls'];
  $widths = $setting ['widths'];
  $heights = $setting ['heights'];
  
  $urls = serialize ($urls);
  $widths = serialize ($widths);
  $heights = serialize ($heights);

  $database_config_user->setWorkbenchURLs ($urls);
  $database_config_user->setWorkbenchWidths ($widths);
  $database_config_user->setWorkbenchHeights ($heights);

} else if ($id == 0) {
} else {
  echo "The server did not recognize the setting and has discarded it";
}


?>
