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
include ("administration/credentials.php");



$database_usfmresources = Database_UsfmResources::getInstance ();


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_usfmresources->deleteResource ($delete);
}


@$convert = $_GET['convert'];
if (isset ($convert)) {
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  $timer_logger = new Timer_Logger ();
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::manual);
  $convert = escapeshellarg ($convert);
  $command = "cd $workingdirectory; php convert2bible.php $convert > $logfilename 2>&1 & echo $!";
  $pid = shell_exec ($command);
  $timer_logger->registerLogfile ($command, $pid, $logfilename, false);
  header ("Location: ../manage/logbook.php");
  die;
}


$header = new Assets_Header (gettext ("USFM Resources"));
$header->jQueryOn ();
$header->jQueryUIOn ("dialog");
$header->run ();
$view = new Assets_View (__FILE__);


$resources = $database_usfmresources->getResources ();
$view->view->resources = $resources;


$view->render ("manage.php");
Assets_Page::footer ();


?>
