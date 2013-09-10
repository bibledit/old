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
$header = new Assets_Header (gettext ("Convert"));
$header->run ();
$view = new Assets_View (__FILE__);


@$bible = $_GET['bible'];
$view->view->bible = $bible;


$database_resources = Database_Resources::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();


$usfmResources = $database_usfmresources->getResources ();
if (in_array ($bible, $usfmResources)) {
  $view->view->error = gettext ("A USFM Resource with this name already exists");
}
$externalResources = $database_resources->getNames ();
if (in_array ($bible, $externalResources)) {
  $view->view->error = gettext ("An external resource with this name already exists");
}


@$convert = $_GET ['convert'];
if (isset ($convert)) {
  $workingdirectory = escapeshellarg (dirname (__FILE__));
  $timer_logger = new Timer_Logger ();
  $logfilename = $timer_logger->getLogFilename (Timer_Logger::manual);
  $bible = escapeshellarg ($bible);
  $command = "cd $workingdirectory; php convert2resource.php $bible > $logfilename 2>&1 & echo $!";
  $pid = shell_exec ($command);
  $timer_logger->registerLogfile ($command, $pid, $logfilename, false);
  header ("Location: ../manage/logbook.php");
  die;
}


$view->render ("bible2resource.php");


Assets_Page::footer ();


?>
