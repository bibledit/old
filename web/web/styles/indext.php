<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);
Assets_Page::header (gettext ("Styles"));
$view = new Assets_View (__FILE__);
$database_config_user = Database_Config_User::getInstance();

if (isset ($_GET['sheet'])) {
  $sheet = $_GET['sheet'];
  if ($sheet == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to switch to another stylesheet?"));
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    foreach ($sheets as $sheet) {
      $parameter = "&sheet=$sheet";
      $dialog_list->add_row ($sheet, $parameter);
    }
    $dialog_list->run ();
  } else {
    $database_config_user->setStylesheet ($sheet);
  }
}

$stylesheet = $database_config_user->getStylesheet();
$view->view->stylesheet = Filter_Html::sanitize ($stylesheet);
$view->render ("indext.php");
Assets_Page::footer ();

?>
