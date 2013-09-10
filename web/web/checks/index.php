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


$database_check = Database_Check::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$view = new Assets_View (__FILE__);


@$goto = $_GET['goto'];
if (isset ($goto)) {
  $passage = $database_check->getPassage ($goto);
  if ($passage != NULL) {
    $ipc_focus = Ipc_Focus::getInstance();
    $ipc_focus->set ($passage['book'], $passage['chapter'], $passage['verse']);
    header ("Location: ../editusfm/index.php");
    die;
  } else {
    $view->view->error = gettext ("The passage for this entry was not found.");
  }
}


Assets_Page::header (gettext ("Checks"));


@$approve = $_GET['approve'];
if (isset ($approve)) {
  $database_check->approve ($approve);
  $view->view->success = gettext ("The entry was approved and suppressed.");
}


@$delete = $_GET['delete'];
if (isset ($delete)) {
  $database_check->delete ($delete);
  $view->view->success = gettext ("The entry was deleted for just now.");
}


$ids = array ();
$data = array ();


$hits = $database_check->getHits ();
foreach ($hits as $hit) {
  $ids [] = $hit['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($hit['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($hit['book'], $hit['chapter'], $hit['verse'])));
  $result = Filter_Html::sanitize ($hit['data']);
  $result = "$bible $passage $result";
  $data [] = $result;
}
$view->view->ids = $ids;
$view->view->data = $data;


$view->render ("index.php");
Assets_Page::footer ();
?>
