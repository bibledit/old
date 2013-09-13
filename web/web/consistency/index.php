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


$database_check = Database_Check::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$ipc_focus = Ipc_Focus::getInstance();


$header = new Assets_Header (gettext ("Consistency"));
$header->jQueryOn ();
$header->run ();
$view = new Assets_View (__FILE__);


@$goto = $_GET['goto'];
if (isset ($goto)) {
  $passage = $database_check->getPassage ($goto);
  if ($passage != NULL) {
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
    header ("Location: ../editusfm/index.php");
    die;
  } else {
    $view->view->error = gettext ("The passage for this entry was not found.");
  }
}


$view->render ("index.php");
Assets_Page::footer ();
?>
