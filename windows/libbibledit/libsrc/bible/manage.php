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


Assets_Page::header (Locale_Translate::_("Bibles"));
$view = new Assets_View (__FILE__);


$database_bibles = Database_Bibles::getInstance ();
$database_users = Database_Users::getInstance ();
$session_logic = Session_Logic::getInstance ();


// Delete Bible handler.
if (isset ($_GET['delete'])) {
  $bible = $_GET['delete'];
  @$confirm = $_GET['confirm'];
  if ($confirm != "") {
    ignore_user_abort (true);
    set_time_limit (0);
    // User needs write access for delete operation.
    if (Access_Bible::write ($bible)) {
      Bible_Logic::deleteBible ($bible);
      $gitdirectory = Filter_Git::git_directory ($bible);
      if (file_exists ($gitdirectory)) {
        Filter_Rmdir::rmdir ($gitdirectory);
      }
    } else {
      Assets_Page::error ("Insufficient privileges to complete action");
    }
  } else {
    $dialog_yes = new Dialog_Yes (NULL, Locale_Translate::_("Would you like to delete Bible $bible?"), "delete");
    die;
  }
}


// New Bible handler.
if (isset ($_GET['new'])) {
  $dialog_entry = new Dialog_Entry ("", Locale_Translate::_("Please enter a name for the new empty Bible"), "", "new", "");
  die;
}
if (isset($_POST['new'])) {
  $bible = $_POST['entry'];
  $bibles = $database_bibles->getBibles ();
  if (in_array ($bible, $bibles)) {
    $error_message = Locale_Translate::_("This Bible already exists");
  } else {
    $database_bibles->createBible ($bible);
    // Check / grant access.
    if (!Access_Bible::write ($bible)) {
      $database_users->grantAccess2Bible ($session_logic->currentUser (), $bible);
    }
  }
}


// Copy Bible handler.
@$copy = $_GET['copy'];
if (isset ($copy)) {
  $dialog_entry = new Dialog_Entry (array ("origin" => $copy), Locale_Translate::_("Please enter a name for where to copy the Bible to"), "", "", "");
  die;
}
@$origin = $_GET['origin'];
if (isset ($origin)) {
  $destination = $_POST['entry'];
  if (isset ($destination)) {
    $bibles = $database_bibles->getBibles ();
    if (in_array ($destination, $bibles)) {
      $error_message = Locale_Translate::_("Cannot copy Bible because the destination Bible already exists.");
    } else {
      ignore_user_abort (true);
      set_time_limit (0);
      // User needs read access to the original.
      if (Access_Bible::read ($origin)) {
        $database_bibles->createBible ($destination);
        $books = $database_bibles->getBooks ($origin);
        foreach ($books as $book) {
          $chapters = $database_bibles->getChapters ($origin, $book);
          foreach ($chapters as $chapter) {
            $data = $database_bibles->getChapter ($origin, $book, $chapter);
            Bible_Logic::storeChapter ($destination, $book, $chapter, $data);
          }
        }
        $success_message = Locale_Translate::_("The Bible was copied.");
        // Check / grant access to destination Bible.
        if (!Access_Bible::write ($destination)) {
          $database_users->grantAccess2Bible ($session_logic->currentUser (), $destination);
        }
      }
    }
  }
}


@$view->view->success_message = $success_message;
@$view->view->error_message = $error_message;
$bibles = Access_Bible::bibles ();
$view->view->bibles = $bibles;
$view->render ("manage.php");
Assets_Page::footer ();


?>
