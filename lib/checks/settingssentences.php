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


require_once ("../bootstrap/bootstrap"); Todo port it.
page_access_level (Filter_Roles::manager ());


Assets_Page::header (translate("Sentence Structure"));
$view = new Assets_View (__FILE__);
$database_config_user = Database_Config_User::getInstance();
$database_config_bible = Database_Config_Bible::getInstance();


$bible = access_bible_clamp (request->database_config_user()->getBible ());


if (isset(request->post['capitals'])) {
  Database_Config_Bible::setSentenceStructureCapitals ($bible, request->post['capitals']);
  $view.set_variable ("success = translate("The capitals were stored");
}


if (isset(request->post['smallletters'])) {
  Database_Config_Bible::setSentenceStructureSmallLetters ($bible, request->post['smallletters']);
  $view.set_variable ("success = translate("The small letters were stored");
}


if (isset(request->post['endpunctuationmarks'])) {
  Database_Config_Bible::setSentenceStructureEndPunctuation ($bible, request->post['endpunctuationmarks']);
  $view.set_variable ("success = translate("The punctuation marks at the ends of sentences were stored");
}


if (isset(request->post['middlepunctuationmarks'])) {
  Database_Config_Bible::setSentenceStructureMiddlePunctuation ($bible, request->post['middlepunctuationmarks']);
  $view.set_variable ("success = translate("The punctuation marks within the sentences were stored");
}


if (isset(request->post['disregards'])) {
  Database_Config_Bible::setSentenceStructureDisregards ($bible, request->post['disregards']);
  $view.set_variable ("success = translate("The characters that should be disregarded within the sentences were stored");
}


if (isset(request->post['names'])) {
  Database_Config_Bible::setSentenceStructureNames ($bible, request->post['names']);
  $view.set_variable ("success = translate("The names that may occur after mid-sentence punctuation were stored");
}


$view.set_variable ("bible = $bible;
$view.set_variable ("capitals = filter_string_sanitize_html (Database_Config_Bible::getSentenceStructureCapitals ($bible));
$view.set_variable ("smallletters = filter_string_sanitize_html (Database_Config_Bible::getSentenceStructureSmallLetters ($bible));
$view.set_variable ("endpunctuationmarks = filter_string_sanitize_html (Database_Config_Bible::getSentenceStructureEndPunctuation ($bible));
$view.set_variable ("middlepunctuationmarks = filter_string_sanitize_html (Database_Config_Bible::getSentenceStructureMiddlePunctuation ($bible));
$view.set_variable ("disregards = filter_string_sanitize_html (Database_Config_Bible::getSentenceStructureDisregards ($bible));
$view.set_variable ("names = filter_string_sanitize_html (Database_Config_Bible::getSentenceStructureNames ($bible));
$view->render ("settingssentences");


Assets_Page::footer ();


?>
