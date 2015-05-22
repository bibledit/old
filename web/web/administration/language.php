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
page_access_level (Filter_Roles::ADMIN_LEVEL);


Assets_Page::header (Locale_Translate::_("Language"));


$database_config_general = Database_Config_General::getInstance ();


@$language =  $_GET['language'];
if (isset ($language)) {
  if ($language == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Set the default language for the site"));
    $languages = Locale_Logic::getLocalizations ();
    foreach ($languages as $language) {
      $dialog_list->add_row (Locale_Logic::getLanguage ($language), "&language=$language");
    }
    $dialog_list->run ();
  } else {
    $database_config_general->setSiteLanguage (Locale_Logic::filterDefault ($language));
  }
}


$view = new Assets_View (__FILE__);
$view->view->language = Locale_Logic::getLanguage ($database_config_general->getSiteLanguage());
$view->render ("language.php");


Assets_Page::footer ();


?>
