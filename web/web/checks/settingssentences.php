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


Assets_Page::header (Locale_Translate::_("Sentence Structure"));
$view = new Assets_View (__FILE__);
$database_config_user = Database_Config_User::getInstance();
$database_config_bible = Database_Config_Bible::getInstance();


$bible = Access_Bible::clamp ($database_config_user->getBible ());


if (isset($_POST['capitals'])) {
  $database_config_bible->setSentenceStructureCapitals ($bible, $_POST['capitals']);
  $view->view->success = Locale_Translate::_("The capitals were stored");
}


if (isset($_POST['smallletters'])) {
  $database_config_bible->setSentenceStructureSmallLetters ($bible, $_POST['smallletters']);
  $view->view->success = Locale_Translate::_("The small letters were stored");
}


if (isset($_POST['endpunctuationmarks'])) {
  $database_config_bible->setSentenceStructureEndPunctuation ($bible, $_POST['endpunctuationmarks']);
  $view->view->success = Locale_Translate::_("The punctuation marks at the ends of sentences were stored");
}


if (isset($_POST['middlepunctuationmarks'])) {
  $database_config_bible->setSentenceStructureMiddlePunctuation ($bible, $_POST['middlepunctuationmarks']);
  $view->view->success = Locale_Translate::_("The punctuation marks within the sentences were stored");
}


if (isset($_POST['disregards'])) {
  $database_config_bible->setSentenceStructureDisregards ($bible, $_POST['disregards']);
  $view->view->success = Locale_Translate::_("The characters that should be disregarded within the sentences were stored");
}


if (isset($_POST['names'])) {
  $database_config_bible->setSentenceStructureNames ($bible, $_POST['names']);
  $view->view->success = Locale_Translate::_("The names that may occur after mid-sentence punctuation were stored");
}


$view->view->bible = $bible;
$view->view->capitals = Filter_Html::sanitize ($database_config_bible->getSentenceStructureCapitals ($bible));
$view->view->smallletters = Filter_Html::sanitize ($database_config_bible->getSentenceStructureSmallLetters ($bible));
$view->view->endpunctuationmarks = Filter_Html::sanitize ($database_config_bible->getSentenceStructureEndPunctuation ($bible));
$view->view->middlepunctuationmarks = Filter_Html::sanitize ($database_config_bible->getSentenceStructureMiddlePunctuation ($bible));
$view->view->disregards = Filter_Html::sanitize ($database_config_bible->getSentenceStructureDisregards ($bible));
$view->view->names = Filter_Html::sanitize ($database_config_bible->getSentenceStructureNames ($bible));
$view->render ("settingssentences.php");


Assets_Page::footer ();


?>
