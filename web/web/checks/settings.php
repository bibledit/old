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
  $database_config_general->setCheckFullStopInHeadings (!$database_config_general->getCheckFullStopInHeadings ());
}
$smarty->assign ("full_stop_headings", $database_config_general->getCheckFullStopInHeadings ());


if (isset ($_GET['spacebeforepunctuation'])) {
  $database_config_general->setCheckSpaceBeforePunctuation (!$database_config_general->getCheckSpaceBeforePunctuation ());
}
$smarty->assign ("space_before_punctuation", $database_config_general->getCheckSpaceBeforePunctuation ());


if (isset ($_GET['sentencestructure'])) {
  $database_config_general->setCheckSentenceStructure (!$database_config_general->getCheckSentenceStructure ());
}
$smarty->assign ("sentence_structure", $database_config_general->getCheckSentenceStructure ());


if (isset ($_GET['chaptersversesversification'])) {
  $database_config_general->setCheckChaptesVersesVersification (!$database_config_general->getCheckChaptesVersesVersification ());
}
$smarty->assign ("chapters_verses_versification", $database_config_general->getCheckChaptesVersesVersification ());


$smarty->assign ("bibles", $database_config_general->getCheckedBibles ());
$smarty->display("settings.tpl");
Assets_Page::footer ();


?>
