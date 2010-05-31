<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


/*
 * Snapshots preserve the history of a chapter being edited.
 * 
 * When a Bible, book or chapter gets deleted, snapshots are not deleted. 
 * If a book was deleted by mistake, it can then be restored easily by consulting the snapshots taken.
*/


class Database_Snapshots
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Snapshots();
    }
    return self::$instance;
  }

  /**
  * Verify database table, optionally creating and/or optimizing it
  */
  public function verify () {
    $database_instance = Database_Instance::getInstance();
$str = <<<EOD
CREATE TABLE IF NOT EXISTS snapshots (
id int auto_increment primary key,
bible int,
book int,
chapter int,
data text,
seconds int,
persistent int
);
EOD;
    $database_instance->runQuery ($str);
  }


  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE snapshots;");
  }

  /**
  * Takes a snapshot of a chapter.
  * $persistent: True or false. 
  * If true, this snapshot will be regarded more persistent than usual, it lasts longer before getting removed.
  */
  public function snapChapter ($name, $book, $chapter, $data, $persistent)
  {
    $database_bibles = Database_Bibles::getInstance();
    $bible_id = $database_bibles->getID ($name);
    if ($bible_id == 0) {
      $database_logs = Database_Logs::getInstance();
      $database_logs->log ("Bible $name does not exist: Failed to snap a chapter");
      return;
    }
    $database_instance = Database_Instance::getInstance();
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $data = Database_SQLInjection::no ($data);
    $time = time ();
    $persistent = Filter_Bool::int ($persistent);
    $query = "INSERT INTO snapshots VALUES (NULL, $bible_id, $book, $chapter, '$data', $time, $persistent);";
    $database_instance->runQuery ($query);
  }


  /**
    * Trims the snapshots.
    * Note: It was tried to optimize this by reading all data into memory and sorting it out from there.
    * But measurements showed that this did not speed up the process, rather it slowed it down.
    */
  public function trim ()
  {
    $current_time = time ();
    $database_instance = Database_Instance::getInstance();

    // Record count.
    $query = "SELECT id FROM snapshots;";
    $result = $database_instance->runQuery ($query);
    $record_count_start = $result->num_rows;
    
    // Go through all chapters in all Bible books in the snapshots.
    $bibles = array ();
    $query = "SELECT DISTINCT bible, book, chapter FROM snapshots;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row();
      $bibles[]   = $row[0];
      $books[]    = $row[1];
      $chapters[] = $row[2];
    }
    foreach ($bibles as $key => $value) {
      $bible   = $bibles  [$key];
      $book    = $books   [$key];
      $chapter = $chapters[$key];

      // Remove duplicate non-persistent snapshots in favour of the persistent ones.
      $query = "SELECT seconds FROM snapshots WHERE bible = $bible AND book = $book AND chapter = $chapter AND persistent = 1;";
      $result = $database_instance->runQuery ($query);
      $persistent_seconds = array();
      for ($i = 0; $i < $result->num_rows; $i++) {
        $row = $result->fetch_row();
        $persistent_seconds[] = $row[0];
      }
      foreach ($persistent_seconds as $seconds) {
        $query = "DELETE FROM snapshots WHERE bible = $bible AND book = $book AND chapter = $chapter AND seconds = $seconds AND persistent = 0;";
        $database_instance->runQuery ($query);
      }
      
      // Get the non-persistent snapshots for this Bible / book / chapter. Youngest ones first.
      // Divide the snapshots into age groups.
      // The persistent snapshots are left untouched.
      $query = "SELECT seconds FROM snapshots WHERE bible = $bible AND book = $book AND chapter = $chapter AND persistent = 0 ORDER BY seconds DESC;";
      $result = $database_instance->runQuery ($query);
      $snapshots = array();
      for ($i = 0; $i < $result->num_rows; $i++) {
        $row = $result->fetch_row();
        $snapshots[] = $row[0];
      }
      $two_days_to_one_week    = array ();
      $one_week_to_one_month   = array ();
      $one_month_to_six_months = array ();
      $six_months_and_more     = array ();
      foreach ($snapshots as $snapshot) {
        $snapshot_age = $current_time - $snapshot;
        // From 0 to 2 days old: Keep everything.
        if ($snapshot_age <= 172800) {
        }
        // From 2 days to 1 week.
        else if (($snapshot_age >= 172800) && ($snapshot_age <= 604800)) {
          $two_days_to_one_week[] = $snapshot;
        }
        // From 1 week to 1 month.
        else if (($snapshot_age >= 604800) && ($snapshot_age <= 2592000)) {
          $one_week_to_one_month[] = $snapshot;
        }
        // From 1 month to 6 months.
        else if (($snapshot_age >= 2592000) && ($snapshot_age <= 15552000)) {
          $one_month_to_six_months[] = $snapshot;
        }
        // Older than 6 months.
        else if ($snapshot_age >= 15552000) {
          $six_months_and_more[] = $snapshot;
        }
      }

      // Trim each group.
      // From 2 days to 1 week: Keep one snapshot per hour.
      $this->trim_by_group ($two_days_to_one_week, $bible, $book, $chapter, 3600);
      // From 1 week to 1 month: Keep one snapshot per day.
      $this->trim_by_group ($one_week_to_one_month, $bible, $book, $chapter, 86400);
      // From 1 month to 6 months: Keep one snapshot every week.
      $this->trim_by_group ($one_month_to_six_months, $bible, $book, $chapter, 604800);
      // Older than 6 months: Keep one snapshot every month.
      $this->trim_by_group ($six_months_and_more, $bible, $book, $chapter, 2592000);
    }

    // Feedback.
    $query = "SELECT id FROM snapshots;";
    $result = $database_instance->runQuery ($query);
    $record_count_end = $result->num_rows;
    include_once ("messages/messages.php");
    $message = "Trimming down snapshots from $record_count_start records to $record_count_end";
    $database_logs = Database_Logs::getInstance();
    $database_logs->log ($message);
  }


  /**
  * Trims snapshots by age group.
  * $group: the group to trim.
  * $spacing: the seconds of space that should be between the snapshots.
  */
  private function trim_by_group ($group, $bible, $book, $chapter, $spacing)
  {
    // Bail out if there are less than 3 snapshots.
    if (count ($group) < 3) {
      return;
    }

    $database_instance = Database_Instance::getInstance();

    // The second of the previous snapshot.
    $previous_second = $group[0];

    // Go through the snapshots, but do not touch the first and last one.
    $maximum = count ($group) - 1;
    for ($i = 1; $i < $maximum; $i++) {

      // Trim the snapshots.
      if ($group[$i] > ($previous_second - $spacing)) {
        $seconds = $group[$i];
        $query = "DELETE FROM snapshots WHERE bible = $bible AND book = $book AND chapter = $chapter AND seconds = $seconds AND persistent = 0;";
        $database_instance->runQuery ($query);
      } else {
        $previous_second = $group[$i];
      }

    }
  }


}



?>
