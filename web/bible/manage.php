<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$database_bibles = Database_Bibles::getInstance();


// Delete Bible handler.
if ($_GET['delete'] != "") {
  $name = $_GET['delete'];
  $confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_bibles->deleteBible ($name);
  } else {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete Bible $name?"), "delete");
    die;
  }
}


// New Bible handler.
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter a name for the new empty Bible"), "", "new", "");
  die;
}
if (isset($_POST['new'])) {
  $name = $_POST['entry'];
  $database_bibles->createBible ($name);
}


$bibles = $database_bibles->getBibles();
$smarty->assign ("bibles", $bibles);
$smarty->display ("manage.tpl");


?>
