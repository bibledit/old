<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::consultant ());


$database_config_user = Database_Config_User::getInstance ();


@$add = request->post['add'];
if (isset ($add)) {
  request->database_config_user()->setActiveWorkbench ($add);
  Workbench_Logic::setURLs    (Workbench_Logic::defaultURLs (0));
  Workbench_Logic::setWidths  (Workbench_Logic::defaultWidths (0));
  Workbench_Logic::setHeights (Workbench_Logic::defaultHeights (0));
}


@$workbenches = request->post ['workbenches'];
if (isset ($workbenches)) {
  $workbenches = explode (",", $workbenches);
  Workbench_Logic::orderWorkbenches ($workbenches);
  die;
}


$header = new Assets_Header (gettext("Workbenches"));
$header->jQueryUIOn ("sortable");
$header->run ();
$view = new Assets_View (__FILE__);


@$remove = request->query['remove'];
if (isset ($remove)) {
  @$confirm = request->query['confirm'];
  if ($confirm != "yes") {
    $dialog_yes = new Dialog_Yes2 (gettext("Would you like to delete this workbench configuration?"), "remove=$remove");
  } else {
    Workbench_Logic::deleteWorkbench ($remove);
  }
}


$workbenches = Workbench_Logic::getWorkbenches ();
$view->view->workbenches = $workbenches;


$view->render ("organize");
Assets_Page::footer ();


?>
