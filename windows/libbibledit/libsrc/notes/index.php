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


$database_notes = Database_Notes::getInstance();
$database_config_user = Database_Config_User::getInstance();
$database_bibles = Database_Bibles::getInstance();
$session_logic = Session_Logic::getInstance ();


// Presets for notes selectors.
// Used by the daily statistics and the workbench.
@$preset_selector = $_GET ['presetselection'];
if (isset ($preset_selector)) {
  $database_config_user->setConsultationNotesPassageSelector (3);
  $database_config_user->setConsultationNotesEditSelector (0);
  $database_config_user->setConsultationNotesNonEditSelector (0);
  $database_config_user->setConsultationNotesStatusSelector ("");
  $database_config_user->setConsultationNotesBibleSelector ("");
  $database_config_user->setConsultationNotesAssignmentSelector ("");
  $database_config_user->setConsultationNotesSubscriptionSelector (0);
  $database_config_user->setConsultationNotesSeveritySelector (-1);
  $database_config_user->setConsultationNotesTextSelector (0);
  switch ($preset_selector) {
    case "assigned":
      $database_config_user->setConsultationNotesAssignmentSelector ($session_logic->currentUser ());
      break;
    case "subscribed":
      $database_config_user->setConsultationNotesSubscriptionSelector (1);
      break;
    case "subscribeddayidle":
      $database_config_user->setConsultationNotesSubscriptionSelector (1);
      $database_config_user->setConsultationNotesNonEditSelector (1);
      break;
    case "subscribedweekidle":
      $database_config_user->setConsultationNotesSubscriptionSelector (1);
      $database_config_user->setConsultationNotesNonEditSelector (3);
      break;
    case "forverse":
      $database_config_user->setConsultationNotesPassageSelector (0);
      break;
  }
}


$header = new Assets_Header (Locale_Translate::_("Consultation Notes"));
$header->setNavigator ();
$header->run();


$view = new Assets_View (__FILE__);


$view->render ("index.php");


Assets_Page::footer ();


?>
