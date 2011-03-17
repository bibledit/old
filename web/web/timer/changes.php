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

$database_bibles = Database_Bibles::getInstance ();
$bibles = $database_bibles->getDiffBibles ();
foreach ($bibles as $bible) {

  // Files get stored in http://site.org/bibledit/downloads/changes/<Bible>/<date>
  // The user can access the files through the browser.
  $biblename = $database_bibles->getName ($bible);
  $directory = dirname (dirname (__FILE__)) . "/downloads/changes/" . $biblename . "/" . strftime ("%Y-%m-%d_%H:%M:%S");
  mkdir ($directory, 0777, true);
  
  // Create symbolic link for easy references.
  $link = dirname ($directory) . "/0_most_recent_changes";
  @unlink ($link);
  symlink ($directory, $link);
  
  // Produce the USFM files.
  Filter_Diff::produceUsfmChapterLevel ($bible, $directory);
  Filter_Diff::produceUsfmVerseLevel ($bible, $directory);
  
  // Run Daisy Diff.
  Filter_Diff::copyDaisyDiffLibraries ($directory);
  Filter_Diff::runDaisyDiff ("$directory/verses_old.usfm", "$directory/verses_new.usfm", "$directory/changed_verses.html"); // Todo
  Filter_Diff::runDaisyDiff ("$directory/chapters_old.usfm", "$directory/chapters_new.usfm", "$directory/changed_chapters.html"); // Todo
  
}

/*
Todo working here.

*/

$database_logs->log (gettext ("The lists of changes in the Bibles have been generated"), true);

?>
