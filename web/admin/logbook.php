<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_logs = Database_Logs::getInstance();
$entries = $database_logs->get ();
while ($row = $entries->fetch_assoc()) {
  $time          = date ('j F Y, g:i a', $row["timestamp"]);
  $timestamps [] = $time;
  $events     [] = $row["event"];
}
$smarty->assign ("timestamps", $timestamps);
$smarty->assign ("events",     $events);
$smarty->display ("logbook.tpl");

?>
