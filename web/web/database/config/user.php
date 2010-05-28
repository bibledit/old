<?php


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
  * Verify database table, optionally creating and/or optimizing it
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS config_user (
id int auto_increment primary key,
username varchar(30),
bible varchar(256),
ident varchar(100) not null,
value varchar(1000),
offset int not null
);
EOD;
    $database_instance->runQuery ($str);
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
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


  public function getStylesheet()
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

  public function getAssignedToConsultationNotesChanges() {
    return $this->getValue ("", "get-assigned-to-consultation-notes-changes", false);
  }
  public function getUserAssignedToConsultationNotesChanges($username) {
    return $this->getValueForUser ($username, "get-assigned-to-consultation-notes-changes", false);
  }
  public function setAssignedToConsultationNotesChanges ($value) {
    $this->setValue ("", "get-assigned-to-consultation-notes-changes", $value);
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
}



?>
