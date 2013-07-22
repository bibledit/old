<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
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
