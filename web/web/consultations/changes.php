<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);
Assets_Page::header (gettext ("Changes"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_changes = Database_Changes::getInstance ();
$session_logic = Session_Logic::getInstance ();


$username = $session_logic->currentUser ();


@$approve = $_GET['approve'];
if (isset ($approve)) {
  $database_changes->delete ($approve);
  $smarty->assign ("success", gettext ("The change was approved."));
}


$ids = $database_changes->getIdentifiers ($username);
$smarty->assign ("ids", $ids);
  

$passages = array ();
foreach ($ids as $id) {
  $passage = $database_changes->getPassage ($id);
  $passage = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
  $passage = Filter_Html::sanitize ($passage);
  $passages [] = $passage;
}
$smarty->assign ("passages", $passages);


$modifications = array ();
foreach ($ids as $id) {
  $modification = $database_changes->getModification ($id);
  $modifications [] = $modification;
}
$smarty->assign ("modifications", $modifications);


$smarty->display("changes.tpl");
Assets_Page::footer ();
?>
