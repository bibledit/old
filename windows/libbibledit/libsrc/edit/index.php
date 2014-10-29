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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$ipc_focus = Ipc_Focus::getInstance();


@$switchbook = $_GET ['switchbook'];
@$switchchapter = $_GET ['switchchapter'];
@$switchverse = $_GET ['switchverse'];
if (isset ($switchbook) && isset ($switchchapter)) {
  $switchbook = Filter_Numeric::integer_in_string ($switchbook);
  $switchchapter = Filter_Numeric::integer_in_string ($switchchapter);
  $verse = 1;
  if (!isset ($switchverse)) $switchverse = 1;
  $ipc_focus->set ($switchbook, $switchchapter, $switchverse);
  Navigation_Passage::recordHistory ($switchbook, $switchchapter, $switchverse);
}


$header = new Assets_Header (Locale_Translate::_("Edit"));
$header->setNavigator ();
$header->setEditorStylesheet ();
$header->run ();


@$changebible = $_GET['changebible'];
if (isset ($changebible)) {
  if ($changebible == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select which Bible to open in the editor"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&changebible=$bible");
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($changebible);
    // Going to another Bible, ensure that the focused book exists there.
    $book = $ipc_focus->getBook ();
    $books = $database_bibles->getBooks ($changebible);
    if (!in_array ($book, $books)) {
      if (count ($books) > 0) $book = $books [0];
      else $book = 0;
      $ipc_focus->set ($book, 1, 1);
    }
  }
}


$view = new Assets_View (__FILE__);


// Active Bible, and check access.
$bible = Access_Bible::clamp ($database_config_user->getBible ());
$view->view->bible = $bible;


// Store the active Bible in the page's javascript.
$view->view->navigationCode = Navigation_Passage::code ($bible);


// Write access?
$write_access = Access_Bible::write ($bible);
$view->view->write_access = $write_access;


$chapterLoaded = Locale_Translate::_("Loaded");
$chapterSaving = Locale_Translate::_("Saving...");
$chapterRetrying = Locale_Translate::_("Retrying...");
$write_access = $write_access ? "true" : "false";
$script = <<<EOD
var editorChapterLoaded = '$chapterLoaded';
var editorChapterSaving = '$chapterSaving';
var editorChapterRetrying = '$chapterRetrying';
var editorWriteAccess = $write_access;
EOD;
$view->view->script = $script;


$class = Filter_CustomCSS::getClass ($bible);
$font = $database_config_bible->getTextFont ($bible);
$direction = $database_config_bible->getTextDirection ($bible);
$view->view->custom_class = $class;
$view->view->custom_css = Filter_CustomCSS::getCss ($class, Fonts_Logic::getFontPath ($font), $direction);


$view->render ("index.php");


Assets_Page::footer ();


/*
Tests for the Bible editor:
* Autosave on going to another passage.
* Autosave on document unload.
* Autosave shortly after any change.
* Automatic reload when another user updates the chapter on the server.
* Position caret at correct verse.
* Scroll caret into view.
*/


?>
