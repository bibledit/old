<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);

@$object = $_GET ['object'];
@$select = $_GET['select'];
if (isset ($select)) {
  if ($select == "") {
    $dialog_list = new Dialog_List (array ("object"), gettext ("Which Bible are you going to use?"), "", "");
    $database_bibles = Database_Bibles::getInstance();
    $bibles = $database_bibles->getBibles();
    foreach ($bibles as $value) {
      $dialog_list->add_row ($value, "&select=$value");
    }
    $dialog_list->run ();
    die;
  } else {
    $object = $select;
  }
}
$smarty->assign ("object", $object);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
if (isset ($_GET ['disable'])) {
  $url = "";
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
  $repository = Filter_Git::git_directory ($object);
  Filter_Rmdir::rmdir ($repository);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

$smarty->display("collaboration.tpl");
?>
