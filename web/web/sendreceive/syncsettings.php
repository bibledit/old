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


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_users = Database_Users::getInstance ();
$session_logic = Session_Logic::getInstance ();


$address = $database_config_general->getServerAddress ();
$url = "$address/sync/settings.php";


// The client is supposed to have one user.
// Get this username to send it to the server.
// The server uses it to access the proper settings for the user.
$users = $database_users->getUsers ();
$user = $users [0];
$session_logic->setUsername ($user);


// The script requests the checksum of all relevant settings from the server.
// It compares this with the local checksum.
// If it matches, that means that the local settings match the settings on the server.
// The script is then ready.
$post = array (
  "a" => "total",
  "u" => bin2hex ($user)
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure synchronizing Settings while requesting totals", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$checksum = Sync_Logic::settings_checksum ();
if ($response == $checksum) {
  $database_logs->log ("The Settings are up to date", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


// At this stage the checksum of all relevant settings on the client differs from the same on the server.
// Requests all settings from the server.
$post = array (
  "a" => Sync_Logic::WORKBENCH_SETTING,
  "u" => bin2hex ($user)
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure receiving Settings", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$urls = $response ['urls'];
$widths = $response ['widths'];
$heights = $response ['heights'];
$urls = serialize ($urls);
$widths = serialize ($widths);
$heights = serialize ($heights);
$database_config_user->setWorkbenchURLs ($urls);
$database_config_user->setWorkbenchWidths ($widths);
$database_config_user->setWorkbenchHeights ($heights);


// Done.
$database_logs->log ("The Settings have been updated", Filter_Roles::TRANSLATOR_LEVEL);


?>
