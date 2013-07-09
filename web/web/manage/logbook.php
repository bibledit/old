<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$database_logs = Database_Logs::getInstance();


if (isset ($_GET['delete'])) {
  $database_logs->clear();
  $database_logs->log (gettext ("Logbook was cleared"), true);
  header ("Location: logbook.php");
  die;
}


@$id = $_GET['id'];
if (isset ($id)) {
  while (true) {
    $result = $database_logs->getID ($id);
    if ($result->num_rows > 0) {
      $row = $result->fetch_assoc ();
      $timestamp = date ('g:i:s a', $row ["timestamp"]);
      $event = Filter_Html::sanitize ($row ["event"]);
      echo "$timestamp | $event";
      die;
    }
    usleep (100000);
  }
}


$header = new Assets_Header (gettext ("Logbook"));
$header->jQueryOn ();
$header->run ();


$view = new Assets_View (__FILE__);


@$day = $_GET['day'];
if (!isset ($day)) {
  $day = 0;
}
$day = (int) $day;


$today = strtotime ("today");
$start = $today - ($day * 86400);
$end = $start + 86400;


$date =  date ('j F Y', $start);
$view->view->date = $date;


$entries = $database_logs->get ($start, $end);
$lines = array ();
while ($row = $entries->fetch_assoc()) {
  $timestamp = date ('g:i:s a', $row ["timestamp"]);
  $event = Filter_Html::sanitize ($row ["event"]);
  $lines [] = "$timestamp | $event";
  $id = $row ["id"];
}
$view->view->lines = $lines;


$nextID = $id + 1;
$script = <<<EOD
var nextID = $nextID;

EOD;
$view->view->script = $script;


$view->render ("logbook.php");
Assets_Page::footer ();


?>
