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
page_access_level (TRANSLATOR_LEVEL);


@$switchbook = $_GET ['switchbook'];
@$switchchapter = $_GET ['switchchapter'];
if (isset ($switchbook) && isset ($switchchapter)) {
  $ipc_focus = Ipc_Focus::getInstance();
  $switchbook = Filter_Numeric::integer_in_string ($switchbook);
  $switchchapter = Filter_Numeric::integer_in_string ($switchchapter);
  $ipc_focus->set ($switchbook, $switchchapter, 1);
}


$header = new Assets_Header (gettext ("Edit USFM"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);


$database_config_user = Database_Config_User::getInstance ();
$bible = $database_config_user->getBible ();


$view->view->navigationHtml = Navigation_Logic::getContainer ();
$view->view->navigationCode = Navigation_Logic::code ($bible);


$chapterLoaded = gettext ("Loaded");
$chapterSaving = gettext ("Saving...");
$script = <<<EOD
var usfmEditorChapterLoaded = '$chapterLoaded';
var usfmEditorChapterSaving = '$chapterSaving';
EOD;
$view->view->script = $script;


$view->render ("index.php");


Assets_Page::footer ();


/*
Tests for the USFM editor:
* Autosave on going to another passage.
* Autosave on document unload.
* Autosave shortly after any change.
*/


?>
