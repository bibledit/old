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
  $smarty->assign ("success", gettext ("The entry was approved and suppressed."));
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_check->delete ($delete);
  $smarty->assign ("success", gettext ("The entry was deleted for just now."));
}


$ids = array ();
$data = array ();


$hits = $database_check->getHits ();
foreach ($hits as $hit) {
  $ids [] = $hit['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($hit['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($hit['book'], $hit['chapter'], $hit['verse'])));
  $result = Filter_Html::sanitize ($hit['data']);
  $result = "$bible $passage $result";
  $data [] = $result;
}
$smarty->assign ("ids", $ids);
$smarty->assign ("data", $data);


$smarty->display("index.tpl");
Assets_Page::footer ();
?>
