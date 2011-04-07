<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Exports"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance();


if (isset($_GET['generate'])) {
  $database_config_general->setTimerExports (time ());
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
    $bibles = $database_config_general->getExportedBibles ();
    $bibles [] = $addbible;
    $bibles = array_unique ($bibles, SORT_STRING);
    $database_config_general->setExportedBibles ($bibles);
  }
}


@$removebible = $_GET['removebible'];
if (isset ($removebible)) {
  $bibles = $database_config_general->getExportedBibles ();
  $bibles = array_diff ($bibles, array ($removebible));
  $bibles = array_values ($bibles);
  $database_config_general->setExportedBibles ($bibles);
  // Remove the Bible's exported data.
  $bibleDirectory = dirname (dirname (__FILE__)) . "/downloads/exports/" . $removebible;
  Filter_Rmdir::rmdir ($bibleDirectory);
}


if (isset ($_GET['sheet'])) {
  $sheet = $_GET['sheet'];
  if ($sheet == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to use another stylesheet for all exports?"));
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    foreach ($sheets as $sheet) {
      $parameter = "&sheet=$sheet";
      $dialog_list->add_row ($sheet, $parameter);
    }
    $dialog_list->run ();
  } else {
    $database_config_general->setExportStylesheet ($sheet);
  }
}


if (isset ($_GET['dropcapstoggle'])) {
  $database_config_general->setExportChapterDropCaps(Filter_Bool::not ($database_config_general->getExportChapterDropCaps()));
}


$smarty->assign ("bibles", $database_config_general->getExportedBibles ());
$smarty->assign ("stylesheet", Filter_Html::sanitize ($database_config_general->getExportStylesheet ()));
$smarty->assign ("dropcaps", $database_config_general->getExportChapterDropCaps());
$smarty->display("exports.tpl");

Assets_Page::footer ();
?>
