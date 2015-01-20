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
page_access_level (Filter_Roles::translator ());


$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_config_general = Database_Config_General::getInstance ();


Assets_Page::header (gettext("Send/Receive"));
$view = new Assets_View (__FILE__);


@$bible = request->query['bible'];
if (isset ($bible)) {
  if ($bible == "") {
    $dialog_list = new Dialog_List2 (gettext("Select a Bible"));
    $bibles = access_bible_bibles ();
    for ($bibles as $bible) {
      // Select Bibles the user has write access to.
      if (access_bible_write ($bible)) {
        $dialog_list->add_row ($bible, "&bible=$bible");
      }
    }
    $dialog_list->run();
  } else {
    request->database_config_user()->setBible ($bible);
  }
}


$bible = access_bible_clamp (request->database_config_user()->getBible ());
$view->view->bible = $bible;


if (isset(request->query['runbible'])) {
  SendReceive_Logic::queuebible ($bible);
  $view->view->successbible = gettext("Will send and receive.");
}


if (isset(request->query['repeatbible'])) {
  Database_Config_Bible::setRepeatSendReceive ($bible, !Database_Config_Bible::getRepeatSendReceive ($bible));
}
$view->view->repeatbible = Database_Config_Bible::getRepeatSendReceive ($bible);


if (Database_Config_Bible::getRemoteRepositoryUrl ($bible) == "") {
  $view->view->errorbible = gettext("Collaboration has not been set up for this Bible");
}


if (isset(request->query['runsync'])) {
  if (SendReceive_Logic::syncqueued ()) {
    $view->view->successnotes = gettext("Still sending and receiving from the last time.");
  } else {
    sendreceive_queue_sync (true);
    $view->view->successnotes = gettext("Will send and receive.");
  }
}


$view->view->client = config_logic_client_enabled ();


if (isset(request->query['repeatsync'])) {
  $repeatsync = request->query['repeatsync'];
  if (!is_numeric ($repeatsync)) $repeatsync = 0;
  if ($repeatsync < 0) $repeatsync = 0;
  if ($repeatsync > 2) $repeatsync = 2;
  Database_Config_General::setRepeatSendReceive ($repeatsync);
}
$view->view->repeatsync = Database_Config_General::getRepeatSendReceive ();


if (Database_Config_General::getServerAddress () == "") {
  $view->view->errornotes = gettext("Collaboration has not been set up for the Bibles and Consultation Notes");
}


$view->view->clientmode = config_logic_client_enabled ();


$view->view->demo = Filter_Demo::client_demo_warning ();


$view->render ("index");


Assets_Page::footer ();


?>
