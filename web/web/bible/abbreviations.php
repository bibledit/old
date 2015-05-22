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


Assets_Page::header (Locale_Translate::_("Abbreviations"));
$view = new Assets_View (__FILE__);


$database_config_bible = Database_Config_Bible::getInstance();


$success_message = "";
$error_message = "";


// The name of the Bible.
$bible = Access_Bible::clamp ($_GET['bible']);
$view->view->bible = Filter_Html::sanitize ($bible);


// Data submission.
if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  $data = trim ($data);
  if ($data != "") {
    if (Validate_Utf8::valid ($data)) {
      $database_config_bible->setBookAbbreviations ($bible, $data);
      $success_message = Locale_Translate::_("The abbreviations were saved.");
    } else {
      $error_message = Locale_Translate::_("Please supply valid Unicode UTF-8 text.");
    }
  } else {
    $success_message = Locale_Translate::_("Nothing was saved.");
  }
}


$data = $database_config_bible->getBookAbbreviations ($bible);
$data = Filter_Abbreviations::display ($data);
$view->view->data = $data;


$view->view->success_message = $success_message;
$view->view->error_message = $error_message;


$view->render ("abbreviations.php");


Assets_Page::footer ();


?>
