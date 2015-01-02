<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
page_access_level (Filter_Roles::manager ());


$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_logs = Database_Logs::getInstance ();


Assets_Page::header (gettext("Export"));
$view = new Assets_View (__FILE__);


@$bible = $_GET['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a Bible"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      // Select Bibles the user has write access to.
      if (access_bible_write ($bible)) {
        $dialog_list->add_row ($bible, "bible=$bible");
      }
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}


$bible = access_bible_clamp ($database_config_user->getBible ());
$view->view->bible = $bible;


if (isset($_GET['remove'])) {
  $directory = Export_Logic::bibleDirectory ($bible);
  filter_url_rmdir ($directory);
  $view->view->success = gettext("The export has been removed."); 
}


if (isset($_GET['webtoggle'])) {
  Database_Config_Bible::setExportWebDuringNight ($bible, !Database_Config_Bible::getExportWebDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to Web format was updated."); 
}
$view->view->web = Database_Config_Bible::getExportWebDuringNight ($bible);


if (isset($_GET['webnow'])) {
  Export_Logic::scheduleWeb ($bible);
  Export_Logic::scheduleWebIndex ($bible);
  $view->view->success = gettext("The Bible is being exported to Web format."); 
}


if (isset($_GET['htmltoggle'])) {
  Database_Config_Bible::setExportHtmlDuringNight ($bible, !Database_Config_Bible::getExportHtmlDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to Html format was updated."); 
}
$view->view->html = Database_Config_Bible::getExportHtmlDuringNight ($bible);


if (isset($_GET['htmlnow'])) {
  Export_Logic::scheduleHtml ($bible);
  $view->view->success = gettext("The Bible is being exported to Html format."); 
}


if (isset($_GET['usfmtoggle'])) {
  Database_Config_Bible::setExportUsfmDuringNight ($bible, !Database_Config_Bible::getExportUsfmDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to USFM format was updated."); 
}
$view->view->usfm = Database_Config_Bible::getExportUsfmDuringNight ($bible);


if (isset($_GET['usfmnow'])) {
  Export_Logic::scheduleUsfm ($bible);
  $view->view->success = gettext("The Bible is being exported to USFM format."); 
}


if (isset($_GET['usfmsecuretoggle'])) {
  Database_Config_Bible::setSecureUsfmExport ($bible, !Database_Config_Bible::getSecureUsfmExport ($bible));
  $view->view->success = gettext("The setting for securing the USFM export was updated."); 
}
$view->view->usfmsecure = Database_Config_Bible::getSecureUsfmExport ($bible);


if (isset($_GET['texttoggle'])) {
  Database_Config_Bible::setExportTextDuringNight ($bible, !Database_Config_Bible::getExportTextDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to basic USFM format and text was updated."); 
}
$view->view->text = Database_Config_Bible::getExportTextDuringNight ($bible);


if (isset($_GET['textnow'])) {
  Export_Logic::scheduleTextAndBasicUsfm ($bible);
  $view->view->success = gettext("The Bible is being exported to basic USFM format and text."); 
}


if (isset($_GET['odttoggle'])) {
  Database_Config_Bible::setExportOdtDuringNight ($bible, !Database_Config_Bible::getExportOdtDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to OpenDocument was updated."); 
}
$view->view->odt = Database_Config_Bible::getExportOdtDuringNight ($bible);


if (isset($_GET['odtnow'])) {
  Export_Logic::scheduleOpenDocument ($bible);
  $view->view->success = gettext("The Bible is being exported to OpenDocument format."); 
}


if (isset ($_GET['dropcapstoggle'])) {
  Database_Config_Bible::setExportChapterDropCapsFrames ($bible, Filter_Bool::not (Database_Config_Bible::getExportChapterDropCapsFrames ($bible)));
}


if (isset ($_GET['pagewidth'])) {
  $dialog_entry = new Dialog_Entry ("", gettext("Please enter a page width in millimeters"), Database_Config_Bible::getPageWidth ($bible), "pagewidth", gettext ("The width of A4 is 210 mm. The width of Letter is 216 mm."));
  die;
}
if (isset($_POST['pagewidth'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 30) && ($value <= 500)) {
    Database_Config_Bible::setPageWidth ($bible, $value);
  }
}


if (isset ($_GET['pageheight'])) {
  $dialog_entry = new Dialog_Entry ("", gettext("Please enter a page height in millimeters"), Database_Config_Bible::getPageHeight ($bible), "pageheight", gettext ("The height of A4 is 297 mm. The width of Letter is 279 mm."));
  die;
}
if (isset($_POST['pageheight'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 40) && ($value <= 600)) {
    Database_Config_Bible::setPageHeight ($bible, $value);
  }
}


if (isset ($_GET['innermargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext("Please enter an inner margin size in millimeters"), Database_Config_Bible::getInnerMargin ($bible), "innermargin", "");
  die;
}
if (isset($_POST['innermargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    Database_Config_Bible::setInnerMargin ($bible, $value);
  }
}


if (isset ($_GET['outermargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext("Please enter an outer margin size in millimeters"), Database_Config_Bible::getOuterMargin ($bible), "outermargin", "");
  die;
}
if (isset($_POST['outermargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    Database_Config_Bible::setOuterMargin ($bible, $value);
  }
}


if (isset ($_GET['topmargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext("Please enter an top margin size in millimeters"), Database_Config_Bible::getTopMargin ($bible), "topmargin", "");
  die;
}
if (isset($_POST['topmargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    Database_Config_Bible::setTopMargin ($bible, $value);
  }
}


if (isset ($_GET['bottommargin'])) {
  $dialog_entry = new Dialog_Entry ("", gettext("Please enter an bottom margin size in millimeters"), Database_Config_Bible::getBottomMargin ($bible), "bottommargin", "");
  die;
}
if (isset($_POST['bottommargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::integer_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    Database_Config_Bible::setBottomMargin ($bible, $value);
  }
}


if (isset ($_GET['dateinheadertoggle'])) {
  Database_Config_Bible::setDateInHeader ($bible, Filter_Bool::not (Database_Config_Bible::getDateInHeader ($bible)));
}


if (isset($_GET['odtsecuretoggle'])) {
  Database_Config_Bible::setSecureOdtExport ($bible, !Database_Config_Bible::getSecureOdtExport ($bible));
  $view->view->success = gettext("The setting for securing the OpenDocument export was updated."); 
}
$view->view->odtsecure = Database_Config_Bible::getSecureOdtExport ($bible);


if (isset($_GET['infotoggle'])) {
  Database_Config_Bible::setGenerateInfoDuringNight ($bible, !Database_Config_Bible::getGenerateInfoDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly generation of info was updated."); 
}
$view->view->info = Database_Config_Bible::getGenerateInfoDuringNight ($bible);


if (isset($_GET['infonow'])) {
  Export_Logic::scheduleInfo ($bible);
  $view->view->success = gettext("The info documents are being generated."); 
}


$view->view->stylesheet = filter_string_sanitize_html (Database_Config_Bible::getExportStylesheet ($bible));
$view->view->dropcaps = Database_Config_Bible::getExportChapterDropCapsFrames ($bible);
$view->view->pagewidth = filter_string_sanitize_html (Database_Config_Bible::getPageWidth ($bible));
$view->view->pageheight = filter_string_sanitize_html (Database_Config_Bible::getPageHeight ($bible));
$view->view->innermargin = filter_string_sanitize_html (Database_Config_Bible::getInnerMargin ($bible));
$view->view->outermargin = filter_string_sanitize_html (Database_Config_Bible::getOuterMargin ($bible));
$view->view->topmargin = filter_string_sanitize_html (Database_Config_Bible::getTopMargin ($bible));
$view->view->bottommargin = filter_string_sanitize_html (Database_Config_Bible::getBottomMargin ($bible));
$view->view->dateinheader = Database_Config_Bible::getDateInHeader ($bible);


if (isset($_GET['eswordtoggle'])) {
  Database_Config_Bible::setExportESwordDuringNight ($bible, !Database_Config_Bible::getExportESwordDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to e-Sword format was updated."); 
}
$view->view->esword = Database_Config_Bible::getExportESwordDuringNight ($bible);


if (isset($_GET['eswordnow'])) {
  Export_Logic::scheduleESword ($bible);
  $view->view->success = gettext("The Bible is being exported to e-Sword format."); 
}


if (isset($_GET['onlinebibletoggle'])) {
  Database_Config_Bible::setExportOnlineBibleDuringNight ($bible, !Database_Config_Bible::getExportOnlineBibleDuringNight ($bible));
  $view->view->success = gettext("The setting for nightly export to Online Bible format was updated."); 
}
$view->view->onlinebible = Database_Config_Bible::getExportOnlineBibleDuringNight ($bible);


if (isset($_GET['onlinebiblenow'])) {
  Export_Logic::scheduleOnlineBible ($bible);
  $view->view->success = gettext("The Bible is being exported to Online Bible format."); 
}


if (isset ($_GET['sheet'])) {
  $sheet = $_GET['sheet'];
  if ($sheet == "") {
    $dialog_list = new Dialog_List2 (gettext("Would you like to use another stylesheet for the exports?"));
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    for ($sheets as $sheet) {
      $parameter = "&sheet=$sheet";
      $dialog_list->add_row ($sheet, $parameter);
    }
    $dialog_list->run ();
  } else {
    Database_Config_Bible::setExportStylesheet ($bible, $sheet);
  }
}


if (isset($_POST['passwordsubmit'])) {
  $password = $_POST['passwordentry'];
  Database_Config_Bible::setExportPassword ($bible, $password);
  $view->view->success = gettext("The password for securing exports was saved."); 
}
$view->view->password = Database_Config_Bible::getExportPassword ($bible);


$view->render ("exports.php");


Assets_Page::footer ();


?>
