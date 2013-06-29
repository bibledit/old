<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


$database_check = Database_Check::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


Assets_Page::header (gettext ("Suppressed check results"));
$view = new Assets_View (__FILE__);


@$release = $_GET['release'];
if (isset($release)) {
  $database_check->release ($release);
  $view->view->success = gettext ("The check result will no longer be suppressed.");
}


$ids = array ();
$data = array ();
$suppressions = $database_check->getSuppressions ();
foreach ($suppressions as $suppression) {
  $ids [] = $suppression['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($suppression['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($suppression['book'], $suppression['chapter'], $suppression['verse'])));
  $result = Filter_Html::sanitize ($suppression['data']);
  $result = "$bible $passage $result";
  $data [] = $result;
}
$view->view->ids = $ids;
$view->view->data = $data;


$view->render ("suppress.php");
Assets_Page::footer ();


?>
