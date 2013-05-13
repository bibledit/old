<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Settings"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_config_general = Database_Config_General::getInstance();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance();
$database_check = Database_Check::getInstance ();


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


if (isset ($_GET['doublespacesusfm'])) {
  $database_config_general->setCheckDoubleSpacesUsfm (!$database_config_general->getCheckDoubleSpacesUsfm ());
}
$smarty->assign ("double_spaces_usfm", $database_config_general->getCheckDoubleSpacesUsfm ());


if (isset ($_GET['fullstopheadings'])) {
  $database_config_general->setFullStopInHeadings (!$database_config_general->getFullStopInHeadings ());
}
$smarty->assign ("full_stop_headings", $database_config_general->getFullStopInHeadings ());


@$release = $_GET['release'];
if (isset($release)) {
  $database_check->release ($release);
  $smarty->assign ("success", gettext ("The check result will no longer be suppressed."));
}


$identifiers = array ();
$results = array ();
$suppressions = $database_check->getSuppressions ();
foreach ($suppressions as $suppression) {
  $identifiers [] = $suppression['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($suppression['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($suppression['book'], $suppression['chapter'], $suppression['verse'])));
  $data = Filter_Html::sanitize ($suppression['data']);
  $result = "$bible $passage $data";
  $results [] = $result;
}
$smarty->assign ("identifiers", $identifiers);
$smarty->assign ("results", $results);


$smarty->assign ("bibles", $database_config_general->getCheckedBibles ());
$smarty->display("settings.tpl");
Assets_Page::footer ();


?>
