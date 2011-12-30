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

$siteUrl = $database_config_general->getSiteURL ();

$bibles = $database_bibles->getDiffBibles ();
foreach ($bibles as $bible) {

  // Files get stored in http://site.org/bibledit/downloads/changes/<Bible>/<date>
  // The files are accessible through the browser.
  $biblename = $database_bibles->getName ($bible);
  $basePath  = "/changes/" . $biblename . "/" . strftime ("%Y-%m-%d_%H:%M:%S");
  $baseUrl = "/downloads" . $basePath;
  $directory = $localStatePath . $basePath;
  mkdir ($directory, 0777, true);
  
  // Remove old symbolic link if it's there.
  $link = dirname ($directory) . "/0_most_recent_changes";
  @unlink ($link);
  
  // Produce the USFM and html files.
  Filter_Diff::produceUsfmChapterLevel ($bible, $directory);
  Filter_Diff::produceVerseLevel ($bible, $directory);
  
  // Delete diff data for this Bible, allowing new diffs to be stored straightaway.
  $database_bibles->deleteDiffBible ($bible);

  // Create online page showing changed verses.
  $versesoutputfile = "$directory/changed_verses.html";
  Filter_Diff::runWDiff ("$directory/verses_old.html", "$directory/verses_new.html", $versesoutputfile);

  // Create online page showing changed chapters.
  $chaptersoutputfile = "$directory/changed_chapters.html";
  Filter_Diff::runWDiff ("$directory/chapters_old.html", "$directory/chapters_new.html", $chaptersoutputfile);
  
  // Insert links to the online versions.
  $links = array ();
  $links [] = array ("", "View this online:");
  $links [] = array ("$siteUrl$baseUrl/changed_verses.html", "changed verses");
  $links [] = array ("", "|");
  $links [] = array ("$siteUrl$baseUrl/changed_chapters.html", "changed chapters");
  $links [] = array ("", "|");
  $links [] = array ("$siteUrl$baseUrl/changed_verses_email.html", "emailed version");
  Filter_Diff::insertLinks ("$directory/changed_verses.html", $links);
  Filter_Diff::insertLinks ("$directory/changed_chapters.html", $links);
  
  // Copy the changed verses to the email file.
  $emailoutputfile = "$directory/changed_verses_email.html";
  file_put_contents ($emailoutputfile, file_get_contents ($versesoutputfile));

  // Email users.
  $subject = gettext ("Recent changes") . " " . $biblename;
  $emailBody = file_get_contents ($emailoutputfile);
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

$command = "chmod -R 0777 $localStatePath/changes 2>&1";
$database_logs->log ($command);
unset ($result);
exec ($command, &$result, &$exit_code);
$database_logs->log (gettext ("The lists of changes in the Bibles have been generated"), true);

?>
