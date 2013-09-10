<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Settings"));
$view = new Assets_View (__FILE__);


$database_config_general = Database_Config_General::getInstance();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance();
$database_check = Database_Check::getInstance ();


if (isset($_GET['run'])) {
  $database_config_general->setTimerChecks (time ());
  $view->view->success = gettext ("Will run the checks within a minute. See the logbook for progress.");
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
$view->view->double_spaces_usfm = $database_config_general->getCheckDoubleSpacesUsfm ();


if (isset ($_GET['fullstopheadings'])) {
  $database_config_general->setCheckFullStopInHeadings (!$database_config_general->getCheckFullStopInHeadings ());
}
$view->view->full_stop_headings = $database_config_general->getCheckFullStopInHeadings ();


if (isset ($_GET['spacebeforepunctuation'])) {
  $database_config_general->setCheckSpaceBeforePunctuation (!$database_config_general->getCheckSpaceBeforePunctuation ());
}
$view->view->space_before_punctuation = $database_config_general->getCheckSpaceBeforePunctuation ();


if (isset ($_GET['sentencestructure'])) {
  $database_config_general->setCheckSentenceStructure (!$database_config_general->getCheckSentenceStructure ());
}
$view->view->sentence_structure = $database_config_general->getCheckSentenceStructure ();


if (isset ($_GET['paragraphstructure'])) {
  $database_config_general->setCheckParagraphStructure (!$database_config_general->getCheckParagraphStructure ());
}
$view->view->paragraph_structure = $database_config_general->getCheckParagraphStructure ();


if (isset ($_GET['chaptersversesversification'])) {
  $database_config_general->setCheckChaptesVersesVersification (!$database_config_general->getCheckChaptesVersesVersification ());
}
$view->view->chapters_verses_versification = $database_config_general->getCheckChaptesVersesVersification ();


if (isset ($_GET['wellformedusfm'])) {
  $database_config_general->setCheckWellFormedUsfm (!$database_config_general->getCheckWellFormedUsfm ());
}
$view->view->well_formed_usfm = $database_config_general->getCheckWellFormedUsfm ();


if (isset ($_GET['punctuationatendverse'])) {
  $database_config_general->setCheckMissingPunctuationEndVerse (!$database_config_general->getCheckMissingPunctuationEndVerse ());
}
$view->view->punctuation_end_verse = $database_config_general->getCheckMissingPunctuationEndVerse ();


if (isset ($_GET['patterns'])) {
  $database_config_general->setCheckPatterns (!$database_config_general->getCheckPatterns ());
}
$view->view->check_patterns = $database_config_general->getCheckPatterns ();


$view->view->bibles = $database_config_general->getCheckedBibles ();
$view->render ("settings.php");
Assets_Page::footer ();


?>
