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

$siteUrl = $database_config_general->getSiteURL ();

$bibles = $database_bibles->getDiffBibles ();
foreach ($bibles as $bible) {

  // Files get stored in http://site.org/bibledit/downloads/changes/<Bible>/<date>
  // The user can access the files through the browser.
  $biblename = $database_bibles->getName ($bible);
  $baseUrl = "/downloads/changes/" . $biblename . "/" . strftime ("%Y-%m-%d_%H:%M:%S");
  $directory = dirname (dirname (__FILE__)) . $baseUrl;
  mkdir ($directory, 0777, true);
  
  // Create symbolic link for easy references.
  $link = dirname ($directory) . "/0_most_recent_changes";
  @unlink ($link);
  symlink ($directory, $link);
  
  // Produce the USFM files.
  Filter_Diff::produceUsfmChapterLevel ($bible, $directory);
  Filter_Diff::produceUsfmVerseLevel ($bible, $directory);
  
  // Delete diff data for this Bible, allowing new diffs to be stored straightaway.
  $database_bibles->deleteDiffBible ($bible);

  // Insert links to all the online versions.
  $links = array ();
  $links [] = array ("", "View this online:");
  $links [] = array ("$siteUrl$baseUrl/changed_verses.html", "changed verses");
  $links [] = array ("", "|");
  $links [] = array ("$siteUrl$baseUrl/changed_chapters.html", "changed chapters");
  $links [] = array ("", "|");
  $links [] = array ("$siteUrl$baseUrl/changed_verses_email.html", "emailed version");
  Filter_Diff::insertLinks ("$directory/verses_old.usfm", $links);
  Filter_Diff::insertLinks ("$directory/verses_new.usfm", $links);
  Filter_Diff::insertLinks ("$directory/chapters_old.usfm", $links);
  Filter_Diff::insertLinks ("$directory/chapters_new.usfm", $links);
  
  // Prepare for Daisy Diff.
  Filter_Diff::copyDaisyDiffLibraries ($directory);

  // Create online page showing the changed verses.
  $outputfile = "$directory/changed_verses.html";
  Filter_Diff::runDaisyDiff ("$directory/verses_old.usfm", "$directory/verses_new.usfm", $outputfile);
  Filter_Diff::setDaisyDiffTitle ($outputfile, $biblename, gettext ("recent changes"));

  // Create online page showing the changed chapters.
  $outputfile = "$directory/changed_chapters.html";
  Filter_Diff::runDaisyDiff ("$directory/chapters_old.usfm", "$directory/chapters_new.usfm", $outputfile);
  Filter_Diff::setDaisyDiffTitle ($outputfile, $biblename, gettext ("recent changes"));
  
  // Create email page showing the changed verses.
  $emailfile = "$directory/changed_verses_email.html";
  Filter_Diff::runDaisyDiff ("$directory/verses_old.usfm", "$directory/verses_new.usfm", $emailfile);
  Filter_Diff::setDaisyDiffTitle ($emailfile, $biblename, gettext ("recent changes"));
  Filter_Diff::setDaisyDiffSubtitle ($emailfile, gettext ("Added text shows in bold and light green. Removed text is stroked out and light red."));
  Filter_Diff::integrateDaisyDiffStylesheet ($emailfile, true);
  Filter_Diff::removeDaisyDiffJavascript ($emailfile);
  Filter_Diff::removeDaisyDiffArrows ($emailfile);

  // Email users.
  $subject = gettext ("Recent changes") . " " . $biblename;
  $emailBody = file_get_contents ($emailfile);
  $users = $database_users->getUsers ();
  foreach ($users as $user) {
    if ($database_config_user->getUserBibleChangesNotification ($user)) {
      $database_mail->send ($user, $subject, $emailBody);
    }
  }
 
}

$database_logs->log (gettext ("The lists of changes in the Bibles have been generated"), true);

?>
