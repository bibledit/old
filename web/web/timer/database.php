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
$database_logs->log ("database: Maintenance has started");

// Security: Page only runs from the cli SAPI.
if (php_sapi_name () != "cli") {
  $database_logs->log ("database: Fatal: This only runs through the cli Server API", true);
  die;
}

$database = Database_Logs::getInstance ();
$database->trim ();
$database->optimize();

$database = Database_Users::getInstance();
$database->optimize ();

$database = Database_Config_General::getInstance ();
$database->optimize ();

$database = Database_Mailer::getInstance ();
$overdues = $database->getOverdueMails ();
foreach ($overdues as $id) $database->delete ($id);
$database->trim ();
$database->optimize ();

$database = Database_Mail::getInstance ();
foreach ($overdues as $id) $database->delete ($id);
unset ($overdues);
$database->trim ();
$database->optimize();

$database = Database_Confirm::getInstance ();
$database->trim ();
$database->optimize ();

$database = Database_Books::getInstance ();
$database->optimize();

$database = Database_Dialog::getInstance ();
$database->optimize ();

$database = Database_Versifications::getInstance();
$database->optimize();

$database = Database_Styles::getInstance();
$database->optimize();

$database = Database_Config_User::getInstance();
$database->optimize();

$database = Database_Bibles::getInstance();
$database->optimize();

$database = Database_Ipc::getInstance();
$database->optimize();

$database = Database_Snapshots::getInstance ();
$database->trim ();
$database->optimize();

$database = Database_Repositories::getInstance();
$database->optimize();

$database = Database_Notes::getInstance();
$database->optimize();

$database = Database_Shell::getInstance();
$database->optimize();

$database = Database_Logger::getInstance ();
$database->optimize();

$database = Database_Check::getInstance ();
$database->optimize();

$database = Database_Changes::getInstance ();
$database->trim ();
$database->optimize();

$database = Database_UsfmResources::getInstance ();
$database->optimize();

$database = Database_Menu::getInstance ();
$database->optimize();

$database = Database_OfflineResources::getInstance ();
$database->optimize();

$database_logs->log ("database: Completed");

?>
