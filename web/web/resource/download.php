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
$header = new Assets_Header (gettext ("Download resource"));
$header->run ();
$view = new Assets_View (__FILE__);


$database_resources = Database_Resources::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();
$timer_logger = new Timer_Logger ();


@$name = $_GET['name'];
$view->view->name = $name;


if (isset ($_GET ['download'])) {
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::manual);
  $name = escapeshellarg ($name);
  $command = "cd $workingdirectory; php downloadcli.php $name > $logfilename 2>&1 & echo $!";
  $pid = shell_exec ($command);
  $timer_logger->registerLogfile ($command, $pid, $logfilename, false);
  header ("Location: ../manage/logbook.php");
  die;
}


if (isset ($_GET ['clear'])) {
  $database_offlineresources->delete ($name);
}


$count = $database_offlineresources->count ($name);
$view->view->count = $count;


$view->render ("download.php");


Assets_Page::footer ();


?>
