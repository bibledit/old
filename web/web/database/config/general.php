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


class Database_Config_General
{


  // Singleton object instance logic.
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Config_General();
    }
    return self::$instance;
  }


  /**
  * Optimize database table
  */
  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE config_general;");
    $database_instance->runQuery ("OPTIMIZE TABLE config_general;");
  }


  // Functions that retrieve the value or list from the database.
  private function getValue ($key, $default) {
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_general WHERE ident = '$key'";
    $result = $database->runQuery ($query);
    if ($result->num_rows == 0) {
      return $default;
    }
    $result_array = $result->fetch_row();
    return $result_array [0];
  }
  private function setValue ($key, $value) {
    $database = Database_Instance::getInstance ();
    $value = Database_SQLInjection::no ($value);
    $query = "DELETE FROM config_general WHERE ident = '$key';";
    $database->runQuery ($query);
    $query = "INSERT INTO config_general VALUES ('$key', '$value', 0);";    
    $database->runQuery ($query);
  }
  private function getList ($key) {
    $database = Database_Instance::getInstance ();
    $query = "SELECT value FROM config_general WHERE ident = '$key' ORDER BY offset";
    $result = $database->runQuery ($query);
    $list = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $result_array = $result->fetch_row();
      $list [] = $result_array [0];
    }
    return $list;
  }
  private function setList ($key, $values) {
    $database = Database_Instance::getInstance ();
    $query = "DELETE FROM config_general WHERE ident = '$key';";
    $database->runQuery ($query);
    foreach ($values as $offset => $value) {
      $offset = Database_SQLInjection::no ($offset);
      $value = Database_SQLInjection::no ($value);
      $query = "INSERT INTO config_general VALUES ('$key', '$value', $offset);";    
      $database->runQuery ($query);
    }
  }


  public function getSiteMailName() {
    return $this->getValue ("site-mail-name", "Bible Translation");
  }
  public function setSiteMailName ($value) {
    $this->setValue ("site-mail-name", $value);
  }

  public function getSiteMailAddress() {
    return $this->getValue ("site-mail-address", "");
  }
  public function setSiteMailAddress ($value) {
    $this->setValue ("site-mail-address", $value);
  }

  public function getMailStorageHost() {
    return $this->getValue ("mail-storage-host", "");
  }
  public function setMailStorageHost ($value) {
    $this->setValue ("mail-storage-host", $value);
  }

  public function getMailStorageUsername() {
    return $this->getValue ("mail-storage-username", "");
  }
  public function setMailStorageUsername ($value) {
    $this->setValue ("mail-storage-username", $value);
  }

  public function getMailStoragePassword() {
    return $this->getValue ("mail-storage-password", "");
  }
  public function setMailStoragePassword ($value) {
    $this->setValue ("mail-storage-password", $value);
  }

  public function getMailStorageSecurity() {
    return $this->getValue ("mail-storage-security", "");
  }
  public function setMailStorageSecurity ($value) {
    $this->setValue ("mail-storage-security", $value);
  }

  public function getMailStoragePort() {
    return $this->getValue ("mail-storage-port", "");
  }
  public function setMailStoragePort ($value) {
    $this->setValue ("mail-storage-port", $value);
  }

  public function getMailSendHost() {
    return $this->getValue ("mail-send-host", "");
  }
  public function setMailSendHost ($value) {
    $this->setValue ("mail-send-host", $value);
  }

  public function getMailSendAuthentication() {
    return $this->getValue ("mail-send-authentication", "");
  }
  public function setMailSendAuthentication ($value) {
    $this->setValue ("mail-send-authentication", $value);
  }

  public function getMailSendUsername() {
    return $this->getValue ("mail-send-username", "");
  }
  public function setMailSendUsername ($value) {
    $this->setValue ("mail-send-username", $value);
  }

  public function getMailSendPassword() {
    return $this->getValue ("mail-send-password", "");
  }
  public function setMailSendPassword ($value) {
    $this->setValue ("mail-send-password", $value);
  }

  public function getMailSendSecurity() {
    return $this->getValue ("mail-send-security", "");
  }
  public function setMailSendSecurity ($value) {
    $this->setValue ("mail-send-security", $value);
  }

  public function getMailSendPort() {
    return $this->getValue ("mail-send-port", "");
  }
  public function setMailSendPort ($value) {
    $this->setValue ("mail-send-port", $value);
  }

  public function getTimerMinute() {
    return $this->getValue ("timer-minute", "");
  }
  public function setTimerMinute ($value) {
    $this->setValue ("timer-minute", $value);
  }

  public function getTimezone() {
    return $this->getValue ("timezone", "UTC");
  }
  public function setTimezone ($value) {
    $this->setValue ("timezone", $value);
  }

  public function getBackupFile() {
    return $this->getValue ("backup-file", "");
  }
  public function setBackupFile ($value) {
    $this->setValue ("backup-file", $value);
  }

  public function getTimerBackup() {
    return $this->getValue ("timer-backup", "");
  }
  public function setTimerBackup ($value) {
    $this->setValue ("timer-backup", $value);
  }

  public function getTimerDiff() {
    return $this->getValue ("timer-diff", "");
  }
  public function setTimerDiff ($value) {
    $this->setValue ("timer-diff", $value);
  }

  public function getTimerExports() {
    return $this->getValue ("timer-exports", "");
  }
  public function setTimerExports ($value) {
    $this->setValue ("timer-exports", $value);
  }

  public function getSiteURL() {
    return $this->getValue ("site-url", "");
  }
  public function setSiteURL ($value) {
    $this->setValue ("site-url", $value);
  }

  public function getExportedBibles() {
    return $this->getList ("exported-bibles", "");
  }
  public function setExportedBibles ($value) {
    $this->setList ("exported-bibles", $value);
  }

  public function getExportStylesheet() {
    return $this->getValue ("export-stylesheet", "Standard");
  }
  public function setExportStylesheet ($value) {
    $this->setValue ("export-stylesheet", $value);
  }

  public function getExportChapterDropCaps() {
    return $this->getValue ("export-chapter-drop-caps", 0);
  }
  public function setExportChapterDropCaps ($value) {
    $this->setValue ("export-chapter-drop-caps", $value);
  }

  public function getHyphenationFirstSet() {
    return $this->getValue ("hyphenation-first-set", "");
  }
  public function setHyphenationFirstSet ($value) {
    $this->setValue ("hyphenation-first-set", $value);
  }

  public function getHyphenationSecondSet() {
    return $this->getValue ("hyphenation-second-set", "");
  }
  public function setHyphenationSecondSet ($value) {
    $this->setValue ("hyphenation-second-set", $value);
  }

  public function getPageWidth () {
    return $this->getValue ("page-width", 210);
  }
  public function setPageWidth  ($value) {
    $this->setValue ("page-width", $value);
  }

  public function getPageHeight () {
    return $this->getValue ("page-height", 297);
  }
  public function setPageHeight  ($value) {
    $this->setValue ("page-height", $value);
  }

  public function getInnerMargin () {
    return $this->getValue ("inner-margin", 20);
  }
  public function setInnerMargin  ($value) {
    $this->setValue ("inner-margin", $value);
  }

  public function getOuterMargin () {
    return $this->getValue ("outer-margin", 10);
  }
  public function setOuterMargin  ($value) {
    $this->setValue ("outer-margin", $value);
  }

  public function getTopMargin () {
    return $this->getValue ("top-margin", 10);
  }
  public function setTopMargin  ($value) {
    $this->setValue ("top-margin", $value);
  }

  public function getBottomMargin () {
    return $this->getValue ("bottom-margin", 10);
  }
  public function setBottomMargin  ($value) {
    $this->setValue ("bottom-margin", $value);
  }

  public function getDateInHeader () {
    return $this->getValue ("date-in-header", false);
  }
  public function setDateInHeader  ($value) {
    $this->setValue ("date-in-header", $value);
  }

  public function getSiteLanguage () {
    return $this->getValue ("site-language", "");
  }
  public function setSiteLanguage ($value) {
    $this->setValue ("site-language", $value);
  }

  public function getTimerChecks () {
    return $this->getValue ("timer-checks", "");
  }
  public function setTimerChecks ($value) {
    $this->setValue ("timer-checks", $value);
  }

  public function getCheckedBibles () {
    return $this->getList ("checked-bibles", "");
  }
  public function setCheckedBibles ($value) {
    $this->setList ("checked-bibles", $value);
  }

  public function getCheckDoubleSpacesUsfm () {
    return $this->getValue ("double-spaces-usfm", "");
  }
  public function setCheckDoubleSpacesUsfm ($value) {
    $this->setValue ("double-spaces-usfm", $value);
  }

  public function getCheckFullStopInHeadings () {
    return $this->getValue ("full-stop-headings", "");
  }
  public function setCheckFullStopInHeadings ($value) {
    $this->setValue ("full-stop-headings", $value);
  }

  public function getCheckSpaceBeforePunctuation () {
    return $this->getValue ("space-before-punctuation", "");
  }
  public function setCheckSpaceBeforePunctuation ($value) {
    $this->setValue ("space-before-punctuation", $value);
  }

  public function getCheckSentenceStructure () {
    return $this->getValue ("sentence-structure", "");
  }
  public function setCheckSentenceStructure ($value) {
    $this->setValue ("sentence-structure", $value);
  }

  public function getCheckParagraphStructure () {
    return $this->getValue ("paragraph-structure", "");
  }
  public function setCheckParagraphStructure ($value) {
    $this->setValue ("paragraph-structure", $value);
  }

  public function getSentenceStructureCapitals () {
    return $this->getValue ("sentence-structure-capitals", "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
  }
  public function setSentenceStructureCapitals ($value) {
    $this->setValue ("sentence-structure-capitals", $value);
  }

  public function getSentenceStructureSmallLetters () {
    return $this->getValue ("sentence-structure-small-letters", "a b c d e f g h i j k l m n o p q r s t u v w x y z");
  }
  public function setSentenceStructureSmallLetters ($value) {
    $this->setValue ("sentence-structure-small-letters", $value);
  }

  public function getSentenceStructureEndPunctuation () {
    return $this->getValue ("sentence-structure-end-punctuation", ". ! ? :");
  }
  public function setSentenceStructureEndPunctuation ($value) {
    $this->setValue ("sentence-structure-end-punctuation", $value);
  }

  public function getSentenceStructureMiddlePunctuation () {
    return $this->getValue ("sentence-structure-middle-punctuation", ", ;");
  }
  public function setSentenceStructureMiddlePunctuation ($value) {
    $this->setValue ("sentence-structure-middle-punctuation", $value);
  }

  public function getSentenceStructureDisregards () {
    return $this->getValue ("sentence-structure-disregards", "( ) [ ] { } ' \" * - 0 1 2 3 4 5 6 7 8 9");
  }
  public function setSentenceStructureDisregards ($value) {
    $this->setValue ("sentence-structure-disregards", $value);
  }

  public function getSentenceStructureNames () {
    return $this->getList ("sentence-structure-names", "");
  }
  public function setSentenceStructureNames ($value) {
    $this->setList ("sentence-structure-names", $value);
  }

  public function getCheckChaptesVersesVersification () {
    return $this->getValue ("check-chapters-verses-versification", "");
  }
  public function setCheckChaptesVersesVersification ($value) {
    $this->setValue ("check-chapters-verses-versification", $value);
  }

  public function getSwordConfiguration () {
    return $this->getList ("sword-configuration", "");
  }
  public function setSwordConfiguration ($value) {
    $this->setList ("sword-configuration", $value);
  }

  public function getCheckWellFormedUsfm () {
    return $this->getValue ("check-well-formed-usfm", "");
  }
  public function setCheckWellFormedUsfm ($value) {
    $this->setValue ("check-well-formed-usfm", $value);
  }

  public function getCheckMissingPunctuationEndVerse () {
    return $this->getValue ("missing-punctuation-end-verse", "");
  }
  public function setCheckMissingPunctuationEndVerse ($value) {
    $this->setValue ("missing-punctuation-end-verse", $value);
  }

  public function getCheckPatterns () {
    return $this->getValue ("check_patterns", "");
  }
  public function setCheckPatterns ($value) {
    $this->setValue ("check_patterns", $value);
  }

  public function getCheckingPatterns () {
    return $this->getList ("checking-patterns", "");
  }
  public function setCheckingPatterns ($value) {
    $this->setList ("checking-patterns", $value);
  }

  public function getRepeatSendReceive () {
    return $this->getValue ("repeat-send-receive", false);
  }
  public function setRepeatSendReceive ($value) {
    $this->setValue ("repeat-send-receive", $value);
  }

}


?>
