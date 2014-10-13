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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_config_general = Database_Config_General::getInstance ();


Assets_Page::header (Locale_Translate::_("Send/Receive"));
$view = new Assets_View (__FILE__);


@$bible = $_GET['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Select a Bible"));
    $bibles = Access_Bible::bibles ();
    foreach ($bibles as $bible) {
      // Select Bibles the user has write access to.
      if (Access_Bible::write ($bible)) {
        $dialog_list->add_row ($bible, "&bible=$bible");
      }
    }
    $dialog_list->run();
  } else {
    $database_config_user->setBible ($bible);
  }
}


$bible = Access_Bible::clamp ($database_config_user->getBible ());
$view->view->bible = $bible;


if (isset($_GET['runbible'])) {
  SendReceive_Logic::queuebible ($bible);
  $view->view->successbible = Locale_Translate::_("Will send and receive.");
}


if (isset($_GET['repeatbible'])) {
  $database_config_bible->setRepeatSendReceive ($bible, !$database_config_bible->getRepeatSendReceive ($bible));
}
$view->view->repeatbible = $database_config_bible->getRepeatSendReceive ($bible);


if ($database_config_bible->getRemoteRepositoryUrl ($bible) == "") {
  $view->view->errorbible = Locale_Translate::_("Collaboration has not been set up for this Bible");
}


if (isset($_GET['runsync'])) {
  if (SendReceive_Logic::syncqueued ()) {
    $view->view->successnotes = Locale_Translate::_("Still sending and receiving from the last time.");
  } else {
    SendReceive_Logic::queuesync (true);
    $view->view->successnotes = Locale_Translate::_("Will send and receive.");
  }
}


$view->view->client = Filter_Client::enabled ();


if (isset($_GET['repeatsync'])) {
  $repeatsync = $_GET['repeatsync'];
  if (!is_numeric ($repeatsync)) $repeatsync = 0;
  if ($repeatsync < 0) $repeatsync = 0;
  if ($repeatsync > 2) $repeatsync = 2;
  $database_config_general->setRepeatSendReceive ($repeatsync);
}
$view->view->repeatsync = $database_config_general->getRepeatSendReceive ();


if ($database_config_general->getServerAddress () == "") {
  $view->view->errornotes = Locale_Translate::_("Collaboration has not been set up for the Bibles and Consultation Notes");
}


$view->view->clientmode = Filter_Client::enabled ();


$view->view->demo = Filter_Demo::client_demo_warning ();


$view->render ("index.php");


Assets_Page::footer ();


?>
