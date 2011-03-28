<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Exports"));
$smarty = new Smarty_Bibledit (__FILE__);


$config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance();


if (isset($_GET['generate'])) {
  $config_general->setTimerExports (time ());
  $smarty->assign ("success", gettext ("The Bibles will be exported soon."));
}


@$addbible = $_GET['addbible'];
if (isset ($addbible)) {
  if ($addbible == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to add a Bible to the list of Bibles that will be exported?"));
    $bibles = $database_bibles->getBibles ();
    foreach ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&addbible=$bible");
    }
    $dialog_list->run();
  } else {
    $bibles = $config_general->getExportedBibles ();
    $bibles [] = $addbible;
    $bibles = array_unique ($bibles, SORT_STRING);
    $config_general->setExportedBibles ($bibles);
  }
}


@$removebible = $_GET['removebible'];
if (isset ($removebible)) {
  $bibles = $config_general->getExportedBibles ();
  $bibles = array_diff ($bibles, array ($removebible));
  $bibles = array_values ($bibles);
  $config_general->setExportedBibles ($bibles);
}


$bibles = $config_general->getExportedBibles ();
$smarty->assign ("bibles", $bibles);
$smarty->display("exports.tpl");
Assets_Page::footer ();
?>
