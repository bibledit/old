<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);
$database_logs = Database_Logs::getInstance();


// Deal with AJAX call for a possible new logbook entry.
@$id = $_GET['id'];
if (isset ($id)) {
  $result = $database_logs->getNext ($id);
  if ($result->num_rows > 0) {
    $row = $result->fetch_assoc ();
    $id = $row ['id'];
    $timestamp = date ('g:i:s a', $row ["timestamp"]);
    $event = Filter_Html::sanitize ($row ["event"]);
    $entry = "$timestamp | $event";
    $data = array ('id' => $id, 'entry' => $entry);
    echo json_encode ($data);
  }
  die;
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
$id = 1;
while ($row = $entries->fetch_assoc()) {
  $timestamp = date ('g:i:s a', $row ["timestamp"]);
  $event = Filter_Html::sanitize ($row ["event"]);
  $lines [] = "$timestamp | $event";
  $id = $row ["id"];
}
$view->view->lines = $lines;


// Pass the highest ID in a script for use by the AJAX calls for getting subsequent logbook entries.
$script = <<<EOD
var lastID = $id;
EOD;
// The AJAX calls for getting more logbook entries only runs when displaying
// items for 'today', not for 'yesterday' and other days.
if ($day != 0) $script = "";
$view->view->script = $script;


$view->render ("logbook.php");
Assets_Page::footer ();


?>
