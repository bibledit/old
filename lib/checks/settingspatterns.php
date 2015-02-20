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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::manager ());


Assets_Page::header (translate("Patterns"));
$view = new Assets_View (__FILE__);
$database_config_user = Database_Config_User::getInstance();
$database_config_bible = Database_Config_Bible::getInstance();


$bible = access_bible_clamp (request->database_config_user()->getBible ());



if (isset(request->post['patterns'])) {
  $patterns = request->post ['patterns'];
  if ($bible) Database_Config_Bible::setCheckingPatterns ($bible, $patterns);
  $view.set_variable ("success = translate("The patterns were saved");
}


$view.set_variable ("bible = $bible;
$view.set_variable ("patterns = Database_Config_Bible::getCheckingPatterns ($bible);
$view->render ("settingspatterns");
Assets_Page::footer ();


?>
