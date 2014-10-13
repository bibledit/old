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
Assets_Page::header (Locale_Translate::_("Import BibleWorks"));


$view = new Assets_View (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();


$success_message = "";
$error_message = "";


// The name of the Bible.
$bible = Access_Bible::clamp ($_GET['bible']);
$view->view->bible = Filter_Html::sanitize ($bible);


// Data submission.
if (isset($_POST['submit'])) {
  // Whether to keep the grammatical tags.
  $tags = isset($_POST['tags']);
  // The BibleWorks text.
  $data = $_POST['data'];
  $data = trim ($data);
  if ($data != "") {
    if (Validate_Utf8::valid ($data)) {
      // Convert the BibleWorks text to USFM.
      $usfm = Filter_Bibleworks::import ($data, $tags);
      // Import the USFM.
      $datafile = tempnam (sys_get_temp_dir(), '');
      file_put_contents ($datafile, $usfm);
      $success_message = Locale_Translate::_("Import has started. See Journal for progress.");
      $workingdirectory = dirname (__FILE__);
      Tasks_Logic::queue (Tasks_Logic::PHP, array ("$workingdirectory/importcli.php", $datafile, $bible));
    } else {
      $error_message = Locale_Translate::_("Please supply valid Unicode UTF-8 text.");
    }
  } else {
    $success_message = Locale_Translate::_("Nothing was imported.");
  }
}


@$view->view->success_message = $success_message;
@$view->view->error_message = $error_message;
$view->render ("import_bibleworks.php");
Assets_Page::footer ();


?>
