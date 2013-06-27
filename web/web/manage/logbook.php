<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$database_logs = Database_Logs::getInstance();


if (isset ($_GET['delete'])) {
  $database_logs->clear();
  $database_logs->log (gettext ("Logbook was cleared"), true);
  header ("Location: logbook.php");
} else {
  header ("Refresh: 5");
}


Assets_Page::header (gettext ("Logbook"));
$view = new Assets_View (__FILE__);


@$page = $_GET['page'];
if (!isset ($page)) {
  $page = 1;
}
if ($page < 1) $page = 1;

$entries = $database_logs->get ($page);
$timestamps = array ();
$events = array ();
while ($row = $entries->fetch_assoc()) {
  $timestamps [] = date ('j F Y, g:i:s a', $row["timestamp"]);
  $events     [] = Filter_Html::sanitize ($row["event"]);
}
$view->view->previous = $page - 1;
$view->view->page = $page;
$view->view->next = $page + 1;
$view->view->timestamps = $timestamps;
$view->view->events = $events;
$view->render ("logbook.php");
Assets_Page::footer ();


?>
