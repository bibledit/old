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
if (isset ($_GET['delete'])) {
  $bible = $_GET['delete'];
  @$confirm = $_GET['confirm'];
  if ($confirm != "") {
    $database_bibles->deleteBible ($bible);
    $gitdirectory = Filter_Git::git_directory ($bible);
    if (file_exists ($gitdirectory)) {
      Filter_Rmdir::rmdir ($gitdirectory);
    }
  } else {
    $dialog_yes = new Dialog_Yes (NULL, gettext ("Would you like to delete Bible $bible?"), "delete");
    die;
  }
}


// New Bible handler.
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", gettext ("Please enter a name for the new empty Bible"), "", "new", "");
  die;
}
if (isset($_POST['new'])) {
  $bible = $_POST['entry'];
  $database_bibles->createBible ($bible);
}


// Copy Bible handler.
@$copy = $_GET['copy'];
if (isset ($copy)) {
  $dialog_entry = new Dialog_Entry (array ("origin" => $copy), gettext ("Please enter a name for where to copy the Bible to"), "", "", "");
  die;
}
@$origin = $_GET['origin'];
if (isset ($origin)) {
  $destination = $_POST['entry'];
  if (isset($destination)) {
    $bibles = $database_bibles->getBibles ();
    if (in_array ($destination, $bibles)) {
      $error_message = gettext ("Cannot copy Bible because the destination Bible already exists.");
    } else {
      $database_snapshots = Database_Snapshots::getInstance();
      $database_bibles->createBible ($destination);
      $books = $database_bibles->getBooks ($origin);
      foreach ($books as $book) {
        $chapters = $database_bibles->getChapters ($origin, $book);
        foreach ($chapters as $chapter) {
          $data = $database_bibles->getChapter ($origin, $book, $chapter);
          $database_bibles->storeChapter ($destination, $book, $chapter, $data);
          $database_snapshots->snapChapter ($destination, $book, $chapter, $data, false);
        }
      }
      $success_message = gettext ("The Bible was copied.");
    }
  }
}


@$smarty->assign ("success_message", $success_message);
@$smarty->assign ("error_message", $error_message);
$bibles = $database_bibles->getBibles();
$smarty->assign ("bibles", $bibles);
$smarty->display ("manage.tpl");


?>
