<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);

$bible = $_GET ['bible'];
$select = $_GET['select'];
if (isset ($select)) {
  if ($select == "") {
    $dialog_list = new Dialog_List (array ("bible"), gettext ("Which Bible are you going to use?"), "", "");
    $database_bibles = Database_Bibles::getInstance();
    $bibles = $database_bibles->getBibles();
    foreach ($bibles as $value) {
      $dialog_list->add_row ($value, "&select=$value");
    }
    $dialog_list->run ();
    die;
  } else {
    $bible = $select;
  }
}
$smarty->assign ("bible", $bible);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($bible);
if (isset ($_GET ['disable'])) {
  $url = "";
  $database_config_user->setRemoteRepositoryUrl ($bible, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($bible);
$smarty->assign ("url", $url);

$smarty->display("collaboration.tpl");
?>
