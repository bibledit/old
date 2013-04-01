<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Edit Note Source"));
$smarty = new Smarty_Bibledit (__FILE__);


@$noteIdentifier = $_GET['identifier'];
if (isset($_POST['identifier'])) {
  $noteIdentifier = $_POST['identifier'];
  $noteIdentifier = Filter_Numeric::integer_in_string ($noteIdentifier);
}


$database_notes = Database_Notes::getInstance ();


if (isset($_POST['data'])) {
  $noteData = $_POST['data'];
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->setContents ($noteIdentifier, $noteData);
    $smarty->assign ("success", gettext ("The note was saved"));
  } else {
    $smarty->assign ("error", gettext ("Unknown Note Identifier"));
  }
}


if ($noteIdentifier != "") {
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->getContents ($noteIdentifier);
    $smarty->assign ("data", $noteData);
  } else {
    $smarty->assign ("error", gettext ("Unknown Note Identifier"));
  }
}


$smarty->assign ("identifier", $noteIdentifier);
$smarty->display("editsource.tpl");


Assets_Page::footer ();


?>
