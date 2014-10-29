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


// Security: Page only runs from the cli SAPI.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_logs->log ("Database maintenance running", Filter_Roles::ADMIN_LEVEL);


// Whether running in client mode.
$database_config_general = Database_Config_General::getInstance ();
$client_mode = Filter_Client::enabled ();


// While VACUUM or REINDEX on a SQLite database are running, 
// querying the database then introduces errors like "database schema has changed".
// Therefore this type of maintenance should not be done automatically.


$database = Database_Users::getInstance ();
$database->trim ();
$database->optimize ();


$database = Database_Mail::getInstance ();
$database->trim ();
$database->optimize ();


$database = Database_Confirm::getInstance ();
$database->trim ();
$database->optimize ();


// No need to optimize the following because it is hardly ever written to.
$database = Database_Books::getInstance ();
$database = Database_Styles::getInstance ();


$database = Database_Bibles::getInstance ();
$database->optimize ();


$database = Database_Ipc::getInstance ();
$database->trim ();


$database = Database_Notes::getInstance ();
$database->trim ();
if (!$client_mode) $database->trim_server ();
$database->optimize ();


$database = Database_Shell::getInstance ();
$database->trim ();
$database->optimize ();


$database = Database_Check::getInstance ();
$database->optimize ();


unlink ("../databases/volatile.sqlite");
$database = Database_Volatile::getInstance ();
$database->create ();


$database = Database_Sprint::getInstance ();
$database->optimize ();


unlink ("../databases/navigation.sqlite");
$database = Database_Navigation::getInstance ();
$database->create ();


$database = Database_History::getInstance ();
$database->optimize ();


$database = Database_Commits::getInstance ();
$database->optimize ();


$database = Database_Jobs::getInstance ();
$database->trim ();
$database->optimize ();


$database = Database_Config_User::getInstance ();
$database->trim ();


?>
