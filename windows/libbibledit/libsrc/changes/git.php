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


$history = "history:";
$database_logs = Database_Logs::getInstance ();
$database_logs->log ("$history Extracting history from the git repository", Filter_Roles::MANAGER_LEVEL);


// Newer git versions would read their config from /root and then generate a fatal error.
// Fix that here by setting the $HOME variable to something git can read.
putenv ("HOME=" . dirname (__FILE__));


$database_bibles = Database_Bibles::getInstance ();
$database_history = Database_History::getInstance ();
$database_commits = Database_Commits::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();


$bibles = $database_bibles->getBibles();
foreach ($bibles as $bible) {

  $stylesheet = $database_config_bible->getExportStylesheet ($bible);

  // The git directory for the Bible.
  $directory = Filter_Git::git_directory ($bible);
  if (!is_dir ($directory)) continue;
  $database_logs->log ("$history Processing directory" . " " . $directory, Filter_Roles::MANAGER_LEVEL);
  $shelldirectory = escapeshellarg ($directory);

  // Retrieve all commits from the git repository.
  // Remove the commits already dealt with.
  $commits = Filter_Git::commits ($directory);
  $commits = array_diff ($commits, $database_commits->get ($bible));

  // Deal with all the remaining commits.
  foreach ($commits as $sha1) {

    // Get author and timestamp.
    $author = Filter_Git::committer ($directory, $sha1);
    $timestamp = Filter_Git::timestamp ($directory, $sha1);
    $datetime = date ('j F Y g:i:s a', $timestamp);

    $database_logs->log ("$history $author $datetime $bible $sha1", Filter_Roles::MANAGER_LEVEL);

    $files = Filter_Git::files ($directory, $sha1);
    foreach ($files as $path) {
      $database_logs->log ("$history $path", Filter_Roles::MANAGER_LEVEL);
      $data = Filter_Git::explodePath ($path);
      if ($data) {
        $book = $data ['book'];
        $chapter = $data ['chapter'];
        $data = Filter_Git::changes ($directory, $sha1, $path);
        $oldusfm = $data ['old'];
        $newusfm = $data ['new'];
        $oldverses = Filter_Usfm::getVerseNumbers ($oldusfm);
        $newverses = Filter_Usfm::getVerseNumbers ($newusfm);
        $verses = array_merge ($oldverses, $newverses);
        $verses = array_unique ($verses);
        sort ($verses, SORT_NUMERIC);
        foreach ($verses as $verse) {
          $old_verse_usfm = Filter_Usfm::getVerseText ($oldusfm, $verse);
          $new_verse_usfm = Filter_Usfm::getVerseText ($newusfm, $verse);
          if ($old_verse_usfm != $new_verse_usfm) {
            $filter_text_old = new Filter_Text ($bible);
            $filter_text_new = new Filter_Text ($bible);
            $filter_text_old->html_text_standard = new Html_Text ("");
            $filter_text_new->html_text_standard = new Html_Text ("");
            $filter_text_old->text_text = new Text_Text ();
            $filter_text_new->text_text = new Text_Text ();
            $filter_text_old->addUsfmCode ($old_verse_usfm);
            $filter_text_new->addUsfmCode ($new_verse_usfm);
            $filter_text_old->run ($stylesheet);
            $filter_text_new->run ($stylesheet);
            $old_html = $filter_text_old->html_text_standard->getInnerHtml ();
            $new_html = $filter_text_new->html_text_standard->getInnerHtml ();
            $old_text = $filter_text_old->text_text->get ();
            $new_text = $filter_text_new->text_text->get ();
            $modification = Filter_Diff::diff ($old_text, $new_text);
            $database_history->record ($timestamp, $author, $bible, $book, $chapter, $verse, $old_html, $modification, $new_html);
          }
        }
      }
    }

    // Store this commit so it won't be combed through next time.
    $database_commits->record ($bible, $sha1);

  }

}


$database_logs->log ("$history Ready", Filter_Roles::MANAGER_LEVEL);


?>
