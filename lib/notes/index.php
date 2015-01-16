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


$database_notes = Database_Notes::getInstance();
$database_config_user = Database_Config_User::getInstance();
$database_bibles = Database_Bibles::getInstance();
$session_logic = Session_Logic::getInstance ();


// Presets for notes selectors.
// Used by the daily statistics and the workbench.
@$preset_selector = request->query ['presetselection'];
if (isset ($preset_selector)) {
  request->database_config_user()->setConsultationNotesPassageSelector (3);
  request->database_config_user()->setConsultationNotesEditSelector (0);
  request->database_config_user()->setConsultationNotesNonEditSelector (0);
  request->database_config_user()->setConsultationNotesStatusSelector ("");
  request->database_config_user()->setConsultationNotesBibleSelector ("");
  request->database_config_user()->setConsultationNotesAssignmentSelector ("");
  request->database_config_user()->setConsultationNotesSubscriptionSelector (0);
  request->database_config_user()->setConsultationNotesSeveritySelector (-1);
  request->database_config_user()->setConsultationNotesTextSelector (0);
  switch ($preset_selector) {
    case "assigned":
      request->database_config_user()->setConsultationNotesAssignmentSelector (request->session_logic()->currentUser ());
      break;
    case "subscribed":
      request->database_config_user()->setConsultationNotesSubscriptionSelector (1);
      break;
    case "subscribeddayidle":
      request->database_config_user()->setConsultationNotesSubscriptionSelector (1);
      request->database_config_user()->setConsultationNotesNonEditSelector (1);
      break;
    case "subscribedweekidle":
      request->database_config_user()->setConsultationNotesSubscriptionSelector (1);
      request->database_config_user()->setConsultationNotesNonEditSelector (3);
      break;
    case "forverse":
      request->database_config_user()->setConsultationNotesPassageSelector (0);
      break;
  }
}


$header = new Assets_Header (gettext("Consultation Notes"));
$header->setNavigator ();
$header->run();


$view = new Assets_View (__FILE__);


$view->render ("index");


Assets_Page::footer ();


?>
