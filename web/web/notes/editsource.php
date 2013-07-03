<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Edit Note Source"));
$view = new Assets_View (__FILE__);


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
    $view->view->success = gettext ("The note was saved");
  } else {
    $view->view->error = gettext ("Unknown Note Identifier");
  }
}


if ($noteIdentifier != "") {
  if ($database_notes->identifierExists ($noteIdentifier)) {
    $noteData = $database_notes->getContents ($noteIdentifier);
    $view->view->data = $noteData;
  } else {
    $view->view->error = gettext ("Unknown Note Identifier");
  }
}


$view->view->identifier = $noteIdentifier;
$view->render ("editsource.php");


Assets_Page::footer ();


?>
