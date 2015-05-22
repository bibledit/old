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


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_users = Database_Users::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_modifications = Database_Modifications::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$session_logic = Session_Logic::getInstance ();


$database_logs->log (Locale_Translate::_("Sending statistics to users"), Filter_Roles::ADMIN_LEVEL);


$siteUrl = $database_config_general->getSiteURL ();


$bibles = $database_bibles->getBibles ();


$users = $database_users->getUsers ();
foreach ($users as $user) {


  $subject = "Bibledit " . Locale_Translate::_("statistics");
  $body = array ();


  if ($database_config_user->getUserPendingChangesNotification ($user)) {
    $ids = $database_modifications->getNotificationIdentifiers ($user);
    $body [] = "<p><a href=\"$siteUrl/changes/changes.php\">" . Locale_Translate::_("Number of change notifications awaiting your approval") . "</a>: " . count ($ids) . "</p>\n";
  }


  if ($database_config_user->getUserAssignedNotesStatisticsNotification ($user)) {
    $ids = $database_notes->selectNotes (
      $bibles, // Bibles.
      0,       // Book
      0,       // Chapter
      0,       // Verse
      3,       // Passage selector.
      0,       // Edit selector.
      0,       // Non-edit selector.
      "",      // Status selector.
      "",      // Bible selector.
      $user,   // Assignment selector.
      0,       // Subscription selector.
      -1,      // Severity selector.
      0,       // Text selector.
      "",      // Search text.
      NULL);   // Limit.
    $body [] = "<p><a href=\"$siteUrl/notes/index.php?presetselection=assigned\">" . Locale_Translate::_("Number of consultation notes assigned to you awaiting your response") . "</a>: " . count ($ids) . "</p>\n";
  }


  if ($database_config_user->getUserSubscribedNotesStatisticsNotification ($user)) {
    $body [] = "<p>" . Locale_Translate::_("Number of consultation notes you are subscribed to") . ":</p>\n";
    $body [] = "<ul>\n";
    $session_logic->setUsername ($user);

    $ids = $database_notes->selectNotes (
      $bibles, // Bible.
      0,       // Book
      0,       // Chapter
      0,       // Verse
      3,       // Passage selector.
      0,       // Edit selector.
      0,       // Non-edit selector.
      "",      // Status selector.
      "",      // Bible selector.
      "",      // Assignment selector.
      1,       // Subscription selector.
      -1,      // Severity selector.
      0,       // Text selector.
      "",      // Search text.
      NULL);   // Limit.
    $body [] = "<li><a href=\"$siteUrl/notes/index.php?presetselection=subscribed\">" . Locale_Translate::_("Total") . "</a>: " . count ($ids) . "</li>\n";
    $ids = $database_notes->selectNotes (
      $bibles, // Bible.
      0,       // Book
      0,       // Chapter
      0,       // Verse
      3,       // Passage selector.
      0,       // Edit selector.
      1,       // Non-edit selector.
      "",      // Status selector.
      "",      // Bible selector.
      "",      // Assignment selector.
      1,       // Subscription selector.
      -1,      // Severity selector.
      0,       // Text selector.
      "",      // Search text.
      NULL);   // Limit.
    $body [] = "<li><a href=\"$siteUrl/notes/index.php?presetselection=subscribeddayidle\">" . Locale_Translate::_("Inactive for a day") . "</a>: " . count ($ids) . "</li>\n";
    $ids = $database_notes->selectNotes (
      $bibles, // Bible.
      0,       // Book
      0,       // Chapter
      0,       // Verse
      3,       // Passage selector.
      0,       // Edit selector.
      3,       // Non-edit selector.
      "",      // Status selector.
      "",      // Bible selector.
      "",      // Assignment selector.
      1,       // Subscription selector.
      -1,      // Severity selector.
      0,       // Text selector.
      "",      // Search text.
      NULL);   // Limit.
    $body [] = "<li><a href=\"$siteUrl/notes/index.php?presetselection=subscribedweekidle\">" . Locale_Translate::_("Inactive for a week") . "</a>: " . count ($ids) . "</li>\n";
    $body [] = "</ul>\n";
    $session_logic->setUsername ("");
  }


  if (count ($body) > 0) {
    $body = implode ("\n", $body);
    if (!Filter_Client::enabled ()) $database_mail->send ($user, $subject, $body);
  }
}


?>
