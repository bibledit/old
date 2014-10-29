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


Assets_Page::header (Locale_Translate::_("Styles"));
$view = new Assets_View (__FILE__);


$database_styles = Database_Styles::getInstance ();
$session_logic = Session_Logic::getInstance ();


$username = $session_logic->currentUser ();
$userlevel = $session_logic->currentLevel ();


@$delete = $_GET['delete'];
if ($delete != "") {
  @$confirm = $_GET['confirm'];
  if ($confirm != "") {
    $write = $database_styles->hasWriteAccess ($username, $delete);
    if ($userlevel >= Filter_Roles::ADMIN_LEVEL) $write = true;
    if ($write) {
      $database_styles->deleteSheet ($delete);
      $database_styles->revokeWriteAccess ("", $delete);
      Assets_Page::success (Locale_Translate::_("The stylesheet has been deleted"));
    }
  } else {
    $dialog_yes = new Dialog_Yes (NULL, Locale_Translate::_("Would you like to delete this stylesheet?"), "delete");
    die;
  }
}
// Delete empty sheet that may have been there.
$database_styles->deleteSheet ("");


if (isset($_POST['new'])) {
  $name = $_POST['entry'];
  if (in_array ($name, $database_styles->getSheets ())) {
    Assets_Page::error (Locale_Translate::_("This stylesheet already exists"));
  } else {
    $database_styles->createSheet ($name);
    $database_styles->grantWriteAccess ($username, $name);
    Styles_Sheets::create_all ();
    Assets_Page::success (Locale_Translate::_("The stylesheet has been created"));
  }
}
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter the name for the new stylesheet"), "", "new", "");
  die;
}


$sheets = $database_styles->getSheets();
$editable = array ();
foreach ($sheets as $sheet) {
  $write = $database_styles->hasWriteAccess ($username, $sheet);
  if ($userlevel >= Filter_Roles::ADMIN_LEVEL) $write = true;
  $editable [] = $write;
}


$view->view->sheets = $sheets;
$view->view->editable = $editable;
$view->render ("indexm.php");


Assets_Page::footer ();


?>
