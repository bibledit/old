<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Exports"));
$smarty = new Smarty_Bibledit (__FILE__);


$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_logs = Database_Logs::getInstance ();


if (isset($_GET['generate'])) {
  $database_config_general->setTimerExports (time ());
  $smarty->assign ("success", gettext ("The Bibles will be exported within a minute."));
  $database_logs->log ("The Bibles will be exported within a minute");
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


if (isset ($_GET['pagewidth'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter a page width in millimeters"), $database_config_general->getPageWidth (), "pagewidth", gettext ("The width of A4 is 210 mm. The width of Letter is 216 mm."));
  die;
}
if (isset($_POST['pagewidth'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 30) && ($value <= 500)) {
    $database_config_general->setPageWidth ($value);
  }
}


if (isset ($_GET['pageheight'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter a page height in millimeters"), $database_config_general->getPageHeight (), "pageheight", gettext ("The height of A4 is 297 mm. The width of Letter is 279 mm."));
  die;
}
if (isset($_POST['pageheight'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 40) && ($value <= 600)) {
    $database_config_general->setPageHeight ($value);
  }
}


if (isset ($_GET['innermargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter an inner margin size in millimeters"), $database_config_general->getInnerMargin (), "innermargin", "");
  die;
}
if (isset($_POST['innermargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_general->setInnerMargin ($value);
  }
}


if (isset ($_GET['outermargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter an outer margin size in millimeters"), $database_config_general->getOuterMargin (), "outermargin", "");
  die;
}
if (isset($_POST['outermargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_general->setOuterMargin ($value);
  }
}


if (isset ($_GET['topmargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter an top margin size in millimeters"), $database_config_general->getTopMargin (), "topmargin", "");
  die;
}
if (isset($_POST['topmargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_general->setTopMargin ($value);
  }
}


if (isset ($_GET['bottommargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter an bottom margin size in millimeters"), $database_config_general->getBottomMargin (), "bottommargin", "");
  die;
}
if (isset($_POST['bottommargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_general->setBottomMargin ($value);
  }
}


if (isset ($_GET['dateinheadertoggle'])) {
  $database_config_general->setDateInHeader(Filter_Bool::not ($database_config_general->getDateInHeader()));
}


if (isset ($_GET ['resetswordconfig'])) {
  $database_config_general->setSwordConfiguration (array ());
}


@$swordconfig = $_POST ['swordconfig'];
if (isset ($swordconfig)) { // Todo
  Sword_Text::saveConfiguration ($swordconfig);
  $smarty->assign ("success", gettext ("The Sword configuration was saved"));
}


$smarty->assign ("bibles", $database_config_general->getExportedBibles ());
$smarty->assign ("stylesheet", Filter_Html::sanitize ($database_config_general->getExportStylesheet ()));
$smarty->assign ("dropcaps", $database_config_general->getExportChapterDropCaps());
$smarty->assign ("pagewidth", Filter_Html::sanitize ($database_config_general->getPageWidth ()));
$smarty->assign ("pageheight", Filter_Html::sanitize ($database_config_general->getPageHeight ()));
$smarty->assign ("innermargin", Filter_Html::sanitize ($database_config_general->getInnerMargin ()));
$smarty->assign ("outermargin", Filter_Html::sanitize ($database_config_general->getOuterMargin ()));
$smarty->assign ("topmargin", Filter_Html::sanitize ($database_config_general->getTopMargin ()));
$smarty->assign ("bottommargin", Filter_Html::sanitize ($database_config_general->getBottomMargin ()));
$smarty->assign ("dateinheader", $database_config_general->getDateInHeader());
$smarty->assign ("swordconfig", Sword_Text::getConfiguration ());
$smarty->display("exports.tpl");

Assets_Page::footer ();
?>
