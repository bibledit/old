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


page_access_level (Filter_Roles::translator ());


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$header = new Assets_Header (translate("Cross references"));
$header->setNavigator ();
$header->setEditorStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);


$success = "";
$error = "";


@$source = request->query['source'];
if (isset ($source)) {
  if ($source == "") {
    $dialog_list = new Dialog_List2 (translate("Select which Bible to use as the source where to read the cross references from"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&source=$bible");
    }
    $dialog_list->run();
  } else {
    request->database_config_user()->setSourceXrefBible ($source);
  }
}


@$target = request->query['target'];
if (isset ($target)) {
  if ($target == "") {
    $dialog_list = new Dialog_List2 (translate("Select which Bible to insert the cross references into"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      if (access_bible_write ($bible)) {
        $dialog_list->add_row ($bible, "&target=$bible");
      }
    }
    $dialog_list->run();
  } else {
    request->database_config_user()->setTargetXrefBible ($target);
  }
}


$source = request->database_config_user()->getSourceXrefBible ();
if (!access_bible_read ($source)) {
  $source = "";
  request->database_config_user()->setSourceXrefBible ($source);
}
$target = request->database_config_user()->getTargetXrefBible ();
if (!access_bible_write ($target)) {
  $target = "";
  request->database_config_user()->setTargetXrefBible ($target);
}
if ($source == "") $source = "--";
if ($target == "") $target = "--";
$view.set_variable ("source = $source;
$view.set_variable ("target = $target;


$view.set_variable ("success = $success;
$view.set_variable ("error = $error;


$view->render ("index");


Assets_Page::footer ();


?>
