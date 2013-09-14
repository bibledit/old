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


$header = new Assets_Header (gettext ("Consistency"));
$header->jQueryOn ();
$header->run ();


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();
$ipc_focus = Ipc_Focus::getInstance();


@$addbible = $_GET ['addbible'];
if (isset ($addbible)) {
  if ($addbible == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to add a Bible?"));
    $bibles = array_merge ($database_bibles->getBibles (), $database_usfmresources->getResources ());
    foreach ($bibles as $bible) {
      $dialog_list->add_row ($bible, "&addbible=$bible");
    }
    $dialog_list->run();
  } else {
    $bibles = $database_config_user->getConsistencyBibles ();
    $bibles [] = $addbible;
    $bibles = array_unique ($bibles, SORT_STRING);
    $database_config_user->setConsistencyBibles ($bibles);
  }
}


@$removebible = $_GET ['removebible'];
if (isset ($removebible)) {
  $bibles = $database_config_user->getConsistencyBibles ();
  $bibles = array_diff ($bibles, array ($removebible));
  $bibles = array_values ($bibles);
  $database_config_user->setConsistencyBibles ($bibles);
}


$view = new Assets_View (__FILE__);


$view->view->bibles = $database_config_user->getConsistencyBibles ();


$view->render ("index.php");
Assets_Page::footer ();
?>
