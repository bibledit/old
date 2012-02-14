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

require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("Generating lists of changes in the Bibles"), true);

// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("Fatal: This only runs through the cli Server API", true);
  die;
}

$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_users = Database_Users::getInstance();
$database_mail = Database_Mail::getInstance();
$database_bibles = Database_Bibles::getInstance ();

include ("paths/paths.php");

$bibles = $database_bibles->getDiffBibles ();
foreach ($bibles as $bible) {

  // The files get stored at http://site.org/bibledit-web/downloads/changes/<Bible>/<date>
  $biblename = $database_bibles->getName ($bible);
  $basePath  = "/changes/" . $biblename . "/" . strftime ("%Y-%m-%d_%H:%M:%S");
  $directory = $localStatePath . $basePath;
  mkdir ($directory, 0777, true);
  
  // Remove old symbolic link if it's there.
  $link = dirname ($directory) . "/0_most_recent_changes";
  @unlink ($link);
  
  // Produce the USFM and html files.
  Filter_Diff::produceVerseLevel ($bible, $directory);
  
  // Delete diff data for this Bible, allowing new diffs to be stored straightaway.
  $database_bibles->deleteDiffBible ($bible);

  // Create online page with changed verses.
  $versesoutputfile = "$directory/changed_verses.html";
  Filter_Diff::runWDiff ("$directory/verses_old.html", "$directory/verses_new.html", $versesoutputfile);

  // Email users.
  $subject = gettext ("Recent changes") . " " . $biblename;
  $emailBody = file_get_contents ($versesoutputfile);
  $users = $database_users->getUsers ();
  foreach ($users as $user) {
    if ($database_config_user->getUserBibleChangesNotification ($user)) {
      $database_mail->send ($user, $subject, $emailBody);
    }
  }
  
  // If there are too many sets of changes, archive the oldest ones.
  $changes_directory = dirname (dirname (__FILE__)) . "/downloads/changes/" . $biblename;
  $archive_directory = "$changes_directory/archive";
  @mkdir ($archive_directory, 0777, true);
  $filenames = array ();
  $modificationtimes = array ();
  foreach (new DirectoryIterator ($changes_directory) as $fileInfo) {
    if($fileInfo->isDot()) continue;
    if($fileInfo->isDir()) {
      $filenames[] = $fileInfo->getFilename();
      $modificationtimes[] = $fileInfo->getMTime();
    }
  }
  array_multisort ($modificationtimes, SORT_DESC, SORT_NUMERIC, $filenames);
  for ($i = 7; $i < count ($filenames); $i++) {
    rename ($changes_directory . "/" . $filenames[$i], $changes_directory . "/archive/" . $filenames[$i]);
    $database_logs->log (gettext ("Archiving older set of changes") . " " . $filenames[$i], true);
  }
 
  
 
}

$database_logs->log (gettext ("The lists of changes in the Bibles have been generated"), true);

?>
