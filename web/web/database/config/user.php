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


class Database_Config_User
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Config_User();
    }
    return self::$instance;
  }

  /**
  * Optimize database table.
  */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE config_user;");
    $database_instance->runQuery ("OPTIMIZE TABLE config_user;");
  }


  private function getValue ($bible, $key, $default) {
    $session_logic = Session_Logic::getInstance();
    $username = $session_logic->currentUser();
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_user WHERE username = '$username' AND bible = '$bible' AND ident = '$key';";
    $result = $database->runQuery ($query);
    if ($result->num_rows == 0) {
      return $default;
    }
    $result_array = $result->fetch_row();
    return $result_array [0];
  }
  private function setValue ($bible, $key, $value) {
    $session_logic = Session_Logic::getInstance();
    $username = $session_logic->currentUser();
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $value = Database_SQLInjection::no ($value);
    $database = Database_Instance::getInstance ();
    $query = "DELETE FROM config_user WHERE username = '$username' AND bible = '$bible' AND ident = '$key';";
    $database->runQuery ($query);
    $query = "INSERT INTO config_user VALUES (NULL, '$username', '$bible', '$key', '$value', 0);";    
    $database->runQuery ($query);
  }
  public function getList ($bible, $key) {
    $session_logic = Session_Logic::getInstance();
    $username = $session_logic->currentUser();
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_user WHERE username = '$username' AND bible = '$bible' AND ident = '$key' ORDER BY offset;";
    $result = $database->runQuery ($query);
    $list = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $result_array = $result->fetch_row();
      $list [] = $result_array [0];
    }
    return $list;
  }
  public function setList ($bible, $key, $values) {
    $session_logic = Session_Logic::getInstance();
    $username = $session_logic->currentUser();
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $database = Database_Instance::getInstance ();
    $query = "DELETE FROM config_user WHERE username = '$username' AND bible = '$bible' AND ident = '$key';";
    $database->runQuery ($query);
    foreach ($values as $offset => $value) {
      $offset = Database_SQLInjection::no ($offset);
      $value = Database_SQLInjection::no ($value);
      $query = "INSERT INTO config_user VALUES (NULL, '$username', '$bible', '$key', '$value', $offset);";    
      $database->runQuery ($query);
    }
  }
  private function getValueForBible ($bible, $key, $default) {
    $bible = Database_SQLInjection::no ($bible);
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_user WHERE bible = '$bible' AND ident = '$key';";
    $result = $database->runQuery ($query);
    if ($result->num_rows == 0) {
      return $default;
    }
    $result_array = $result->fetch_row();
    return $result_array [0];
  }
  private function setValueForBible ($bible, $key, $value) {
    $bible = Database_SQLInjection::no ($bible);
    $value = Database_SQLInjection::no ($value);
    $database = Database_Instance::getInstance ();
    $query = "DELETE FROM config_user WHERE bible = '$bible' AND ident = '$key';";
    $database->runQuery ($query);
    $query = "INSERT INTO config_user VALUES (NULL, '', '$bible', '$key', '$value', 0);";    
    $database->runQuery ($query);
  }


  private function getValueForUser ($username, $key, $default) {
    $username = Database_SQLInjection::no ($username);
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_user WHERE username = '$username' AND ident = '$key';";
    $result = $database->runQuery ($query);
    if ($result->num_rows == 0) {
      return $default;
    }
    $result_array = $result->fetch_row();
    return $result_array [0];
  }


  public function getStylesheet ()
  { 
    $sheet = $this->getValue ("", "stylesheet", "Standard");
    // If the stylesheet does not exist, take the first one available instead.
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    if (!in_array ($sheet, $sheets)) {
      $sheet = $sheets[0];
      $this->setStylesheet ($sheet);
    }
    return $sheet;
  }
  public function setStylesheet ($sheet)
  {
    $this->setValue ("", "stylesheet", $sheet);
  }   
  
  
  public function getBible ()
  {
    $bible = $this->getValue ("", "bible", "");
    // If the Bible does not exist, take the first one available.
    $database_bibles = Database_Bibles::getInstance();
    $bibles = $database_bibles->getBibles ();
    if (!in_array ($bible, $bibles)) {
      $bible = $bibles[0];
      $this->setBible ($bible);
    }
    return $bible;
  }
  public function setBible ($bible)
  {
    $this->setValue ("", "bible", $bible);
  }   
 
 
  public function getRemoteRepositoryUrl ($bible)
  {
    return $this->getValueForBible ($bible, "remote-repo-url", "");
  }
  public function setRemoteRepositoryUrl ($bible, $url)
  {
    $this->setValueForBible ($bible, "remote-repo-url", $url);
  }
    
  public function getTimezone() {
    return $this->getValue ("", "timezone", "");
  }
  public function setTimezone ($value) {
    $this->setValue ("", "timezone", $value);
  }   

  public function getSubscribeToConsultationNotesEditedByMe() {
    return $this->getValue ("", "subscribe-to-consultation-notes-edited-by-me", "");
  }
  public function setSubscribeToConsultationNotesEditedByMe ($value) {
    $this->setValue ("", "subscribe-to-consultation-notes-edited-by-me", $value);
  }   

  public function getNotifyMeOfAnyConsultationNotesEdits() {
    return $this->getValue ("", "notify-me-of-any-consultation-notes-edits", "");
  }
  public function getNotifyUserOfAnyConsultationNotesEdits($username) {
    return $this->getValueForUser ($username, "notify-me-of-any-consultation-notes-edits", "");
  }
  public function setNotifyMeOfAnyConsultationNotesEdits ($value) {
    $this->setValue ("", "notify-me-of-any-consultation-notes-edits", $value);
  }   

  public function getSubscribedConsultationNoteNotification() {
    return $this->getValue ("", "subscribed-consultation-note-notification", true);
  }
  public function getUserSubscribedConsultationNoteNotification($username) {
    return $this->getValueForUser ($username, "subscribed-consultation-note-notification", true);
  }
  public function setSubscribedConsultationNoteNotification ($value) {
    $this->setValue ("", "subscribed-consultation-note-notification", $value);
  }   

  public function getAssignedToConsultationNotesChanges() {
    return $this->getValue ("", "get-assigned-to-consultation-notes-changes", false);
  }
  public function getUserAssignedToConsultationNotesChanges($username) {
    return $this->getValueForUser ($username, "get-assigned-to-consultation-notes-changes", false);
  }
  public function setAssignedToConsultationNotesChanges ($value) {
    $this->setValue ("", "get-assigned-to-consultation-notes-changes", $value);
  }   

  public function getGenerateChangeNotifications() {
    return $this->getValue ("", "generate-change-notifications", false);
  }
  public function getUserGenerateChangeNotifications($username) {
    return $this->getValueForUser ($username, "generate-change-notifications", false);
  }
  public function setGenerateChangeNotifications ($value) {
    $this->setValue ("", "generate-change-notifications", $value);
  }   

  public function getAssignedConsultationNoteNotification() {
    return $this->getValue ("", "assigned-consultation-note-notification", true);
  }
  public function getUserAssignedConsultationNoteNotification($username) {
    return $this->getValueForUser ($username, "assigned-consultation-note-notification", true);
  }
  public function setAssignedConsultationNoteNotification ($value) {
    $this->setValue ("", "assigned-consultation-note-notification", $value);
  }   

  // 0: current verse; 1: current chapter; 2: current book; 3: any passage.
  public function getConsultationNotesPassageSelector() {
    return $this->getValue ("", "consultation-notes-passage-selector", 0);
  }
  public function setConsultationNotesPassageSelector ($value) {
    $this->setValue ("", "consultation-notes-passage-selector", $value);
  }   

  // 0: any time; 1: last 30 days; 2: last 7 days; 3: since yesterday; 4: today.
  public function getConsultationNotesEditSelector() {
    return $this->getValue ("", "consultation-notes-edit-selector", 0);
  }
  public function setConsultationNotesEditSelector ($value) {
    $this->setValue ("", "consultation-notes-edit-selector", $value);
  }   

  // 0: don't care; 1: for last 30 days; 2: for last 7 days; 3: since yesterday; 4: today.
  public function getConsultationNotesNonEditSelector() {
    return $this->getValue ("", "consultation-notes-non-edit-selector", 0);
  }
  public function setConsultationNotesNonEditSelector ($value) {
    $this->setValue ("", "consultation-notes-non-edit-selector", $value);
  }   

  // Status is a string; can be empty as well.
  public function getConsultationNotesStatusSelector() {
    return $this->getValue ("", "consultation-notes-status-selector", "");
  }
  public function setConsultationNotesStatusSelector ($value) {
    $this->setValue ("", "consultation-notes-status-selector", $value);
  }   

  // "": any Bible; <bible>: named Bible.
  public function getConsultationNotesBibleSelector() {
    return $this->getValue ("", "consultation-notes-bible-selector", "");
  }
  public function setConsultationNotesBibleSelector ($value) {
    $this->setValue ("", "consultation-notes-bible-selector", $value);
  }   

  // "": don't care; "user": notes assigned to "user".
  public function getConsultationNotesAssignmentSelector() {
    return $this->getValue ("", "consultation-notes-assignment-selector", "");
  }
  public function setConsultationNotesAssignmentSelector ($value) {
    $this->setValue ("", "consultation-notes-assignment-selector", $value);
  }   

  // 0: don't care; 1: subscribed.
  public function getConsultationNotesSubscriptionSelector() {
    return $this->getValue ("", "consultation-notes-subscription-selector", 0);
  }
  public function setConsultationNotesSubscriptionSelector ($value) {
    $this->setValue ("", "consultation-notes-subscription-selector", $value);
  }   

  public function getConsultationNotesSeveritySelector() {
    return $this->getValue ("", "consultation-notes-severity-selector", -1);
  }
  public function setConsultationNotesSeveritySelector ($value) {
    $this->setValue ("", "consultation-notes-severity-selector", $value);
  }   

  public function getConsultationNotesTextSelector() {
    return $this->getValue ("", "consultation-notes-text-selector", 0);
  }
  public function setConsultationNotesTextSelector ($value) {
    $this->setValue ("", "consultation-notes-text-selector", $value);
  }   

  public function getConsultationNotesSearchText() {
    return $this->getValue ("", "consultation-notes-search-text", "");
  }
  public function setConsultationNotesSearchText ($value) {
    $this->setValue ("", "consultation-notes-search-text", $value);
  }   

  public function getConsultationNotesPassageInclusionSelector() {
    return $this->getValue ("", "consultation-notes-passage-inclusion-selector", 0);
  }
  public function setConsultationNotesPassageInclusionSelector ($value) {
    $this->setValue ("", "consultation-notes-passage-inclusion-selector", $value);
  }   

  public function getConsultationNotesTextInclusionSelector() {
    return $this->getValue ("", "consultation-notes-text-inclusion-selector", 0);
  }
  public function setConsultationNotesTextInclusionSelector ($value) {
    $this->setValue ("", "consultation-notes-text-inclusion-selector", $value);
  }

  public function getBibleChangesNotification() {
    return $this->getValue ("", "bible-changes-notification", false);
  }
  public function getUserBibleChangesNotification($username) {
    return $this->getValueForUser ($username, "bible-changes-notification", false);
  }
  public function setBibleChangesNotification ($value) {
    $this->setValue ("", "bible-changes-notification", $value);
  }   

  public function getDeletedConsultationNoteNotification() {
    return $this->getValue ("", "deleted-consultation-note-notification", false);
  }
  public function getUserDeletedConsultationNoteNotification($username) {
    return $this->getValueForUser ($username, "deleted-consultation-note-notification", false);
  }
  public function setDeletedConsultationNoteNotification ($value) {
    $this->setValue ("", "deleted-consultation-note-notification", $value);
  }   

  public function getBibleChecksNotification () {
    return $this->getValue ("", "bible-checks-notification", false);
  }
  public function getUserBibleChecksNotification ($username) {
    return $this->getValueForUser ($username, "bible-checks-notification", false);
  }
  public function setBibleChecksNotification ($value) {
    $this->setValue ("", "bible-checks-notification", $value);
  }   

  public function getPendingChangesNotification () {
    return $this->getValue ("", "pending-changes-notification", false);
  }
  public function getUserPendingChangesNotification ($username) {
    return $this->getValueForUser ($username, "pending-changes-notification", false);
  }
  public function setPendingChangesNotification ($value) {
    $this->setValue ("", "pending-changes-notification", $value);
  }   

  public function getUserChangesNotification () {
    return $this->getValue ("", "user-changes-notification", false);
  }
  public function getUserUserChangesNotification ($username) {
    return $this->getValueForUser ($username, "user-changes-notification", false);
  }
  public function setUserChangesNotification ($value) {
    $this->setValue ("", "user-changes-notification", $value);
  }   

  public function getAssignedNotesStatisticsNotification () {
    return $this->getValue ("", "assigned-notes-statistics-notification", false);
  }
  public function getUserAssignedNotesStatisticsNotification ($username) {
    return $this->getValueForUser ($username, "assigned-notes-statistics-notification", false);
  }
  public function setAssignedNotesStatisticsNotification ($value) {
    $this->setValue ("", "assigned-notes-statistics-notification", $value);
  }   

  public function getSubscribedNotesStatisticsNotification () {
    return $this->getValue ("", "subscribed-notes-statistics-notification", false);
  }
  public function getUserSubscribedNotesStatisticsNotification ($username) {
    return $this->getValueForUser ($username, "subscribed-notes-statistics-notification", false);
  }
  public function setSubscribedNotesStatisticsNotification ($value) {
    $this->setValue ("", "subscribed-notes-statistics-notification", $value);
  }   

  public function getNotifyMeOfMyPosts () {
    return $this->getValue ("", "notify-me-of-my-posts", true);
  }
  public function getUserNotifyMeOfMyPosts ($username) {
    return $this->getValueForUser ($username, "notify-me-of-my-posts", true);
  }
  public function setNotifyMeOfMyPosts ($value) {
    $this->setValue ("", "notify-me-of-my-posts", $value);
  }   

  public function getSuppressMailFromYourUpdatesNotes () {
    return $this->getValue ("", "suppress-mail-my-updated-notes", false);
  }
  public function getUserSuppressMailFromYourUpdatesNotes ($username) {
    return $this->getValueForUser ($username, "suppress-mail-my-updated-notes", false);
  }
  public function setSuppressMailFromYourUpdatesNotes ($value) {
    $this->setValue ("", "suppress-mail-my-updated-notes", $value);
  }   

  public function getActiveResources () {
    return $this->getList ("", "active-resources");
  }
  public function setActiveResources ($values) {
    $this->setList ("", "active-resources", $values);
  }   

  public function getConsistencyBibles () {
    return $this->getList ("", "consistency-bibles");
  }
  public function setConsistencyBibles ($values) {
    $this->setList ("", "consistency-bibles", $values);
  }   

  public function getSprintMonth () {
    return $this->getValue ("", "sprint-month", date ("n"));
  }
  public function setSprintMonth ($value) {
    $this->setValue ("", "sprint-month", $value);
  }   

  public function getSprintYear () {
    return $this->getValue ("", "sprint-year", date ("Y"));
  }
  public function setSprintYear ($value) {
    $this->setValue ("", "sprint-year", $value);
  }   

  public function getSprintProgressNotification () {
    return $this->getValue ("", "sprint-progress-notification", false);
  }
  public function getUserSprintProgressNotification ($username) {
    return $this->getValueForUser ($username, "sprint-progress-notification", false);
  }
  public function setSprintProgressNotification ($value) {
    $this->setValue ("", "sprint-progress-notification", $value);
  }   

  public function getUserChangesNotificationsOnline () {
    return $this->getValue ("", "user-changes-notifications-online", false);
  }
  public function getUserUserChangesNotificationsOnline ($username) {
    return $this->getValueForUser ($username, "user-changes-notifications-online", false);
  }
  public function setUserChangesNotificationsOnline ($value) {
    $this->setValue ("", "user-changes-notifications-online", $value);
  }   

}



?>
