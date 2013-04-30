<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Settings"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_config_general = Database_Config_General::getInstance();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance();


if (isset($_GET['run'])) {
  $database_config_general->setTimerChecks (time ());
  $smarty->assign ("success", gettext ("Will run the checks within a minute. See the logbook for progress."));
  $database_logs->log (gettext ("Will run the checks within a minute"));
}


@$addbible = $_GET['addbible'];
if (isset ($addbible)) {
  if ($addbible == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to add a Bible to the ones that will be checked?"));
    $bibles = $database_bibles->getBibles ();
    foreach ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&addbible=$bible");
    }
    $dialog_list->run();
  } else {
    $bibles = $database_config_general->getCheckedBibles ();
    $bibles [] = $addbible;
    $bibles = array_unique ($bibles, SORT_STRING);
    $database_config_general->setCheckedBibles ($bibles);
  }
}


@$removebible = $_GET['removebible'];
if (isset ($removebible)) {
  $bibles = $database_config_general->getCheckedBibles ();
  $bibles = array_diff ($bibles, array ($removebible));
  $bibles = array_values ($bibles);
  $database_config_general->setCheckedBibles ($bibles);
}


$smarty->assign ("bibles", $database_config_general->getCheckedBibles ());
$smarty->display("settings.tpl");
Assets_Page::footer ();


?>
