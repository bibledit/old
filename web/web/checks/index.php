<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


$database_check = Database_Check::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$smarty = new Smarty_Bibledit (__FILE__);


@$goto = $_GET['goto'];
if (isset ($goto)) {
  $passage = $database_check->getPassage ($goto);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
    header ("Location: ../desktop/index.php?desktop=edittext");
    die;
  } else {
    $smarty->assign ("error", gettext ("The passage for this entry was not found."));
  }
}


Assets_Page::header (gettext ("Checks"));


@$approve = $_GET['approve'];
if (isset ($approve)) {
  $database_check->approve ($approve);
  $smarty->assign ("success", gettext ("The entry was approved."));
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_check->delete ($delete);
  $smarty->assign ("success", gettext ("The entry was deleted for just now."));
}


@$release = $_GET['release'];
if (isset($release)) {
  $database_check->release ($release);
  $smarty->assign ("success", gettext ("The check result will no longer be suppressed."));
}


$result_ids = array ();
$result_data = array ();


$hits = $database_check->getHits ();
foreach ($hits as $hit) {
  $result_ids [] = $hit['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($hit['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($hit['book'], $hit['chapter'], $hit['verse'])));
  $data = Filter_Html::sanitize ($hit['data']);
  $result = "$bible $passage $data";
  $result_data [] = $result;
}
$smarty->assign ("resultcount", count ($result_ids));
$smarty->assign ("result_ids", $result_ids);
$smarty->assign ("result_data", $result_data);


$suppressed_ids = array ();
$suppressed_data = array ();
$suppressions = $database_check->getSuppressions ();
foreach ($suppressions as $suppression) {
  $suppressed_ids [] = $suppression['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($suppression['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($suppression['book'], $suppression['chapter'], $suppression['verse'])));
  $data = Filter_Html::sanitize ($suppression['data']);
  $result = "$bible $passage $data";
  $suppressed_data [] = $result;
}
$smarty->assign ("suppressedcount", count ($suppressed_ids));
$smarty->assign ("suppressed_ids", $suppressed_ids);
$smarty->assign ("suppressed_data", $suppressed_data);




$smarty->display("index.tpl");
Assets_Page::footer ();
?>
