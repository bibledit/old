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


Assets_Page::header (gettext ("Suppressed check results"));
$view = new Assets_View (__FILE__);


@$release = $_GET['release'];
if (isset($release)) {
  $database_check->release ($release);
  $view->view->success = gettext ("The check result will no longer be suppressed.");
}


$ids = array ();
$data = array ();
$suppressions = $database_check->getSuppressions ();
foreach ($suppressions as $suppression) {
  $ids [] = $suppression['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($suppression['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($suppression['book'], $suppression['chapter'], $suppression['verse'])));
  $result = Filter_Html::sanitize ($suppression['data']);
  $result = "$bible $passage $result";
  $data [] = $result;
}
$view->view->ids = $ids;
$view->view->data = $data;


$view->render ("suppress.php");
Assets_Page::footer ();


?>
