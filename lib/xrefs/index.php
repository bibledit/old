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
$database_bibles = Database_Bibles::getInstance ();


$header = new Assets_Header (Locale_Translate::_("Cross references"));
$header->setNavigator ();
$header->setEditorStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);


$success = "";
$error = "";


@$source = $_GET['source'];
if (isset ($source)) {
  if ($source == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select which Bible to use as the source where to read the cross references from"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&source=$bible");
    }
    $dialog_list->run();
  } else {
    $database_config_user->setSourceXrefBible ($source);
  }
}


@$target = $_GET['target'];
if (isset ($target)) {
  if ($target == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select which Bible to insert the cross references into"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      if (Access_Bible::write ($bible)) {
        $dialog_list->add_row ($bible, "&target=$bible");
      }
    }
    $dialog_list->run();
  } else {
    $database_config_user->setTargetXrefBible ($target);
  }
}


$source = $database_config_user->getSourceXrefBible ();
if (!Access_Bible::read ($source)) {
  $source = "";
  $database_config_user->setSourceXrefBible ($source);
}
$target = $database_config_user->getTargetXrefBible ();
if (!Access_Bible::write ($target)) {
  $target = "";
  $database_config_user->setTargetXrefBible ($target);
}
if ($source == "") $source = "--";
if ($target == "") $target = "--";
$view->view->source = $source;
$view->view->target = $target;


$view->view->success = $success;
$view->view->error = $error;


$view->render ("index.php");


Assets_Page::footer ();


?>
