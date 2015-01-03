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


Assets_Page::header (gettext("Manage Checks"));
$view = new Assets_View (__FILE__);


$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_check = Database_Check::getInstance ();


@$bible = request->query['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (gettext("Select which Bible to manage"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&bible=$bible");
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}
$bible = access_bible_clamp ($database_config_user->getBible ());


if (isset(request->query['run'])) {
  Checks_Logic::start ($bible);
  $view->view->success = gettext("Will run the checks. See the Journal for progress.");
}


if (isset (request->query['doublespacesusfm'])) {
  Database_Config_Bible::setCheckDoubleSpacesUsfm ($bible, !Database_Config_Bible::getCheckDoubleSpacesUsfm ($bible));
}
$view->view->double_spaces_usfm = Database_Config_Bible::getCheckDoubleSpacesUsfm ($bible);


if (isset (request->query['fullstopheadings'])) {
  Database_Config_Bible::setCheckFullStopInHeadings ($bible, !Database_Config_Bible::getCheckFullStopInHeadings ($bible));
}
$view->view->full_stop_headings = Database_Config_Bible::getCheckFullStopInHeadings ($bible);


if (isset (request->query['spacebeforepunctuation'])) {
  Database_Config_Bible::setCheckSpaceBeforePunctuation ($bible, !Database_Config_Bible::getCheckSpaceBeforePunctuation ($bible));
}
$view->view->space_before_punctuation = Database_Config_Bible::getCheckSpaceBeforePunctuation ($bible);


if (isset (request->query['sentencestructure'])) {
  Database_Config_Bible::setCheckSentenceStructure ($bible, !Database_Config_Bible::getCheckSentenceStructure ($bible));
}
$view->view->sentence_structure = Database_Config_Bible::getCheckSentenceStructure ($bible);


if (isset (request->query['paragraphstructure'])) {
  Database_Config_Bible::setCheckParagraphStructure ($bible, !Database_Config_Bible::getCheckParagraphStructure ($bible));
}
$view->view->paragraph_structure = Database_Config_Bible::getCheckParagraphStructure ($bible);


if (isset (request->query['chaptersversesversification'])) {
  Database_Config_Bible::setCheckChaptesVersesVersification ($bible, !Database_Config_Bible::getCheckChaptesVersesVersification ($bible));
}
$view->view->chapters_verses_versification = Database_Config_Bible::getCheckChaptesVersesVersification ($bible);


if (isset (request->query['wellformedusfm'])) {
  Database_Config_Bible::setCheckWellFormedUsfm ($bible, !Database_Config_Bible::getCheckWellFormedUsfm ($bible));
}
$view->view->well_formed_usfm = Database_Config_Bible::getCheckWellFormedUsfm ($bible);


if (isset (request->query['punctuationatendverse'])) {
  Database_Config_Bible::setCheckMissingPunctuationEndVerse ($bible, !Database_Config_Bible::getCheckMissingPunctuationEndVerse ($bible));
}
$view->view->punctuation_end_verse = Database_Config_Bible::getCheckMissingPunctuationEndVerse ($bible);


if (isset (request->query['patterns'])) {
  Database_Config_Bible::setCheckPatterns ($bible, !Database_Config_Bible::getCheckPatterns ($bible));
}
$view->view->check_patterns = Database_Config_Bible::getCheckPatterns ($bible);


$view->view->bible = $bible;
$view->render ("settings.php");
Assets_Page::footer ();


?>
