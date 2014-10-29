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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


@$id = $_GET ["id"];


// Get information about this job.
$database_jobs = Database_Jobs::getInstance ();
$exists = $database_jobs->idExists ($id);
$level = $database_jobs->getLevel ($id);
$progress = $database_jobs->getProgress ($id);
$result = $database_jobs->getResult ($id);


// Access control for the user.
$session_logic = Session_Logic::getInstance ();
$userlevel = $session_logic->currentLevel ();


if (!$exists) {
  // Check on existence of the job.
  $contents = Locale_Translate::_("This job does not exist.");
} else if ($level > $userlevel) {
  // Check user access to the job.
  $contents = Locale_Translate::_("This job is not available to you.");
} else if ($result != "") {
  $contents = $result;
} else if ($progress != "") {
  $contents = $progress;
} else {
  $contents = Locale_Translate::_("The job is in the queue.");
}


// If the result is still pending, refresh the page regularly.
if ($result == "") {
  header ("Refresh: 3");
}


$header = new Assets_Header (Locale_Translate::_("Job"));
$header->setEditorStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);
$view->view->contents = $contents;
$view->render ("index.php");


Assets_Page::footer ();


?>
