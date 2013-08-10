<?php


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
