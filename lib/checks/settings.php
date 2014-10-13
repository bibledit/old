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


Assets_Page::header (Locale_Translate::_("Manage Checks"));
$view = new Assets_View (__FILE__);


$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_check = Database_Check::getInstance ();


@$bible = $_GET['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select which Bible to manage"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&bible=$bible");
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}
$bible = Access_Bible::clamp ($database_config_user->getBible ());


if (isset($_GET['run'])) {
  Checks_Logic::start ($bible);
  $view->view->success = Locale_Translate::_("Will run the checks. See the Journal for progress.");
}


if (isset ($_GET['doublespacesusfm'])) {
  $database_config_bible->setCheckDoubleSpacesUsfm ($bible, !$database_config_bible->getCheckDoubleSpacesUsfm ($bible));
}
$view->view->double_spaces_usfm = $database_config_bible->getCheckDoubleSpacesUsfm ($bible);


if (isset ($_GET['fullstopheadings'])) {
  $database_config_bible->setCheckFullStopInHeadings ($bible, !$database_config_bible->getCheckFullStopInHeadings ($bible));
}
$view->view->full_stop_headings = $database_config_bible->getCheckFullStopInHeadings ($bible);


if (isset ($_GET['spacebeforepunctuation'])) {
  $database_config_bible->setCheckSpaceBeforePunctuation ($bible, !$database_config_bible->getCheckSpaceBeforePunctuation ($bible));
}
$view->view->space_before_punctuation = $database_config_bible->getCheckSpaceBeforePunctuation ($bible);


if (isset ($_GET['sentencestructure'])) {
  $database_config_bible->setCheckSentenceStructure ($bible, !$database_config_bible->getCheckSentenceStructure ($bible));
}
$view->view->sentence_structure = $database_config_bible->getCheckSentenceStructure ($bible);


if (isset ($_GET['paragraphstructure'])) {
  $database_config_bible->setCheckParagraphStructure ($bible, !$database_config_bible->getCheckParagraphStructure ($bible));
}
$view->view->paragraph_structure = $database_config_bible->getCheckParagraphStructure ($bible);


if (isset ($_GET['chaptersversesversification'])) {
  $database_config_bible->setCheckChaptesVersesVersification ($bible, !$database_config_bible->getCheckChaptesVersesVersification ($bible));
}
$view->view->chapters_verses_versification = $database_config_bible->getCheckChaptesVersesVersification ($bible);


if (isset ($_GET['wellformedusfm'])) {
  $database_config_bible->setCheckWellFormedUsfm ($bible, !$database_config_bible->getCheckWellFormedUsfm ($bible));
}
$view->view->well_formed_usfm = $database_config_bible->getCheckWellFormedUsfm ($bible);


if (isset ($_GET['punctuationatendverse'])) {
  $database_config_bible->setCheckMissingPunctuationEndVerse ($bible, !$database_config_bible->getCheckMissingPunctuationEndVerse ($bible));
}
$view->view->punctuation_end_verse = $database_config_bible->getCheckMissingPunctuationEndVerse ($bible);


if (isset ($_GET['patterns'])) {
  $database_config_bible->setCheckPatterns ($bible, !$database_config_bible->getCheckPatterns ($bible));
}
$view->view->check_patterns = $database_config_bible->getCheckPatterns ($bible);


$view->view->bible = $bible;
$view->render ("settings.php");
Assets_Page::footer ();


?>
