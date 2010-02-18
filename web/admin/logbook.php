<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$database_logs = Database_Logs::getInstance();

if ($_GET['delete'] != "") {
  $database_logs->clear();
}

$smarty = new Smarty_Bibledit (__FILE__);
$entries = $database_logs->get ();
while ($row = $entries->fetch_assoc()) {
  $time          = date ('j F Y, g:i:s a', $row["timestamp"]);
  $timestamps [] = $time;
  $events     [] = htmlentities ($row["event"]);
}
$smarty->assign ("timestamps", $timestamps);
$smarty->assign ("events",     $events);
$smarty->display ("logbook.tpl");

?>
