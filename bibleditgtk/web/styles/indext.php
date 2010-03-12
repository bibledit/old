<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_config_user = Database_Config_User::getInstance();

if (isset ($_GET['sheet'])) {
  $sheet = $_GET['sheet'];
  if ($sheet == "") {
    $dialog_list = new Dialog_List (NULL, gettext ("Would you like to switch to another stylesheet?"), "", "");
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    foreach ($sheets as $sheet) {
      $parameter = "?sheet=$sheet";
      $dialog_list->add_row ($sheet, $parameter);
    }
    $dialog_list->run ();
    die;
  } else {
    $database_config_user->setStylesheet ($sheet);
  }
}

$stylesheet = $database_config_user->getStylesheet();
$smarty->assign ("stylesheet", Filter_Html::sanitize ($stylesheet));
$smarty->display ("indext.tpl");

?>
