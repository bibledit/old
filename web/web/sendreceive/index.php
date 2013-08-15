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


$database_config_general = Database_Config_General::getInstance ();


Assets_Page::header (gettext ("Send/Receive"));
$view = new Assets_View (__FILE__);


if (isset($_GET['run'])) {
  if (SendReceive_Logic::isRunning ()) {
    $view->view->success = gettext ("Sending and receiving is in progress.");
  } else {
    SendReceive_Logic::start ();
    $view->view->success = gettext ("Sending and receiving has started.");
  }
}


if (isset($_GET['togglerepeat'])) {
  $database_config_general->setRepeatSendReceive (!$database_config_general->getRepeatSendReceive ());
}
$view->view->repeat = $database_config_general->getRepeatSendReceive ();


$view->render ("index.php");
Assets_Page::footer ();


?>
