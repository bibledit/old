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
page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Styles"));
$view = new Assets_View (__FILE__);
$database_styles = Database_Styles::getInstance();


@$delete = $_GET['delete'];
if ($delete != "") {
  @$confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_styles->deleteSheet ($delete);
  } else {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete this stylesheet?"), "delete");
    die;
  }
}


if (isset($_POST['new'])) {
  $name = $_POST['entry'];
  $database_styles->createSheet ($name);
}


if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter the name for the new stylesheet"), "", "new", "");
  die;
}


$sheets = $database_styles->getSheets();
$view->view->sheets = $sheets;
$view->render ("indexm.php");


Assets_Page::footer ();
?>
