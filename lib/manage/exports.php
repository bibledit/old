<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MANAGER_LEVEL);


$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_logs = Database_Logs::getInstance ();


Assets_Page::header (Locale_Translate::_("Export"));
$view = new Assets_View (__FILE__);


@$bible = $_GET['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a Bible"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      // Select Bibles the user has write access to.
      if (Access_Bible::write ($bible)) {
        $dialog_list->add_row ($bible, "bible=$bible");
      }
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}


$bible = Access_Bible::clamp ($database_config_user->getBible ());
$view->view->bible = $bible;


if (isset($_GET['remove'])) {
  $directory = Export_Logic::bibleDirectory ($bible);
  Filter_Rmdir::rmdir ($directory);
  $view->view->success = Locale_Translate::_("The export has been removed."); 
}


if (isset($_GET['webtoggle'])) {
  $database_config_bible->setExportWebDuringNight ($bible, !$database_config_bible->getExportWebDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to Web format was updated."); 
}
$view->view->web = $database_config_bible->getExportWebDuringNight ($bible);


if (isset($_GET['webnow'])) {
  Export_Logic::scheduleWeb ($bible);
  Export_Logic::scheduleWebIndex ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to Web format."); 
}


if (isset($_GET['htmltoggle'])) {
  $database_config_bible->setExportHtmlDuringNight ($bible, !$database_config_bible->getExportHtmlDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to Html format was updated."); 
}
$view->view->html = $database_config_bible->getExportHtmlDuringNight ($bible);


if (isset($_GET['htmlnow'])) {
  Export_Logic::scheduleHtml ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to Html format."); 
}


if (isset($_GET['usfmtoggle'])) {
  $database_config_bible->setExportUsfmDuringNight ($bible, !$database_config_bible->getExportUsfmDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to USFM format was updated."); 
}
$view->view->usfm = $database_config_bible->getExportUsfmDuringNight ($bible);


if (isset($_GET['usfmnow'])) {
  Export_Logic::scheduleUsfm ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to USFM format."); 
}


if (isset($_GET['usfmsecuretoggle'])) {
  $database_config_bible->setSecureUsfmExport ($bible, !$database_config_bible->getSecureUsfmExport ($bible));
  $view->view->success = Locale_Translate::_("The setting for securing the USFM export was updated."); 
}
$view->view->usfmsecure = $database_config_bible->getSecureUsfmExport ($bible);


if (isset($_GET['texttoggle'])) {
  $database_config_bible->setExportTextDuringNight ($bible, !$database_config_bible->getExportTextDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to basic USFM format and text was updated."); 
}
$view->view->text = $database_config_bible->getExportTextDuringNight ($bible);


if (isset($_GET['textnow'])) {
  Export_Logic::scheduleTextAndBasicUsfm ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to basic USFM format and text."); 
}


if (isset($_GET['odttoggle'])) {
  $database_config_bible->setExportOdtDuringNight ($bible, !$database_config_bible->getExportOdtDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to OpenDocument was updated."); 
}
$view->view->odt = $database_config_bible->getExportOdtDuringNight ($bible);


if (isset($_GET['odtnow'])) {
  Export_Logic::scheduleOpenDocument ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to OpenDocument format."); 
}


if (isset ($_GET['dropcapstoggle'])) {
  $database_config_bible->setExportChapterDropCapsFrames ($bible, Filter_Bool::not ($database_config_bible->getExportChapterDropCapsFrames ($bible)));
}


if (isset ($_GET['pagewidth'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter a page width in millimeters"), $database_config_bible->getPageWidth ($bible), "pagewidth", Locale_Translate::_ ("The width of A4 is 210 mm. The width of Letter is 216 mm."));
  die;
}
if (isset($_POST['pagewidth'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 30) && ($value <= 500)) {
    $database_config_bible->setPageWidth ($bible, $value);
  }
}


if (isset ($_GET['pageheight'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter a page height in millimeters"), $database_config_bible->getPageHeight ($bible), "pageheight", Locale_Translate::_ ("The height of A4 is 297 mm. The width of Letter is 279 mm."));
  die;
}
if (isset($_POST['pageheight'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 40) && ($value <= 600)) {
    $database_config_bible->setPageHeight ($bible, $value);
  }
}


if (isset ($_GET['innermargin'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter an inner margin size in millimeters"), $database_config_bible->getInnerMargin ($bible), "innermargin", "");
  die;
}
if (isset($_POST['innermargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_bible->setInnerMargin ($bible, $value);
  }
}


if (isset ($_GET['outermargin'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter an outer margin size in millimeters"), $database_config_bible->getOuterMargin ($bible), "outermargin", "");
  die;
}
if (isset($_POST['outermargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_bible->setOuterMargin ($bible, $value);
  }
}


if (isset ($_GET['topmargin'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter an top margin size in millimeters"), $database_config_bible->getTopMargin ($bible), "topmargin", "");
  die;
}
if (isset($_POST['topmargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_bible->setTopMargin ($bible, $value);
  }
}


if (isset ($_GET['bottommargin'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter an bottom margin size in millimeters"), $database_config_bible->getBottomMargin ($bible), "bottommargin", "");
  die;
}
if (isset($_POST['bottommargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $database_config_bible->setBottomMargin ($bible, $value);
  }
}


if (isset ($_GET['dateinheadertoggle'])) {
  $database_config_bible->setDateInHeader ($bible, Filter_Bool::not ($database_config_bible->getDateInHeader ($bible)));
}


if (isset($_GET['odtsecuretoggle'])) {
  $database_config_bible->setSecureOdtExport ($bible, !$database_config_bible->getSecureOdtExport ($bible));
  $view->view->success = Locale_Translate::_("The setting for securing the OpenDocument export was updated."); 
}
$view->view->odtsecure = $database_config_bible->getSecureOdtExport ($bible);


if (isset($_GET['infotoggle'])) {
  $database_config_bible->setGenerateInfoDuringNight ($bible, !$database_config_bible->getGenerateInfoDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly generation of info was updated."); 
}
$view->view->info = $database_config_bible->getGenerateInfoDuringNight ($bible);


if (isset($_GET['infonow'])) {
  Export_Logic::scheduleInfo ($bible);
  $view->view->success = Locale_Translate::_("The info documents are being generated."); 
}


$view->view->stylesheet = Filter_Html::sanitize ($database_config_bible->getExportStylesheet ($bible));
$view->view->dropcaps = $database_config_bible->getExportChapterDropCapsFrames ($bible);
$view->view->pagewidth = Filter_Html::sanitize ($database_config_bible->getPageWidth ($bible));
$view->view->pageheight = Filter_Html::sanitize ($database_config_bible->getPageHeight ($bible));
$view->view->innermargin = Filter_Html::sanitize ($database_config_bible->getInnerMargin ($bible));
$view->view->outermargin = Filter_Html::sanitize ($database_config_bible->getOuterMargin ($bible));
$view->view->topmargin = Filter_Html::sanitize ($database_config_bible->getTopMargin ($bible));
$view->view->bottommargin = Filter_Html::sanitize ($database_config_bible->getBottomMargin ($bible));
$view->view->dateinheader = $database_config_bible->getDateInHeader ($bible);


if (isset($_GET['eswordtoggle'])) {
  $database_config_bible->setExportESwordDuringNight ($bible, !$database_config_bible->getExportESwordDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to e-Sword format was updated."); 
}
$view->view->esword = $database_config_bible->getExportESwordDuringNight ($bible);


if (isset($_GET['eswordnow'])) {
  Export_Logic::scheduleESword ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to e-Sword format."); 
}


if (isset($_GET['onlinebibletoggle'])) {
  $database_config_bible->setExportOnlineBibleDuringNight ($bible, !$database_config_bible->getExportOnlineBibleDuringNight ($bible));
  $view->view->success = Locale_Translate::_("The setting for nightly export to Online Bible format was updated."); 
}
$view->view->onlinebible = $database_config_bible->getExportOnlineBibleDuringNight ($bible);


if (isset($_GET['onlinebiblenow'])) {
  Export_Logic::scheduleOnlineBible ($bible);
  $view->view->success = Locale_Translate::_("The Bible is being exported to Online Bible format."); 
}


if (isset ($_GET['sheet'])) {
  $sheet = $_GET['sheet'];
  if ($sheet == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Would you like to use another stylesheet for the exports?"));
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    foreach ($sheets as $sheet) {
      $parameter = "&sheet=$sheet";
      $dialog_list->add_row ($sheet, $parameter);
    }
    $dialog_list->run ();
  } else {
    $database_config_bible->setExportStylesheet ($bible, $sheet);
  }
}


if (isset($_POST['passwordsubmit'])) {
  $password = $_POST['passwordentry'];
  $database_config_bible->setExportPassword ($bible, $password);
  $view->view->success = Locale_Translate::_("The password for securing exports was saved."); 
}
$view->view->password = $database_config_bible->getExportPassword ($bible);


$view->render ("exports.php");


Assets_Page::footer ();


?>
