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
$database_logs->log ("Generating lists of changes in the Bibles", Filter_Roles::TRANSLATOR_LEVEL);


// Recreate modifications database.
Database_Modifications::delete ();
$database_modifications = Database_Modifications::getInstance ();
$database_modifications->create ();


// Other databases needed.
$database_config_general = Database_Config_General::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_history = Database_History::getInstance ();
$database_users = Database_Users::getInstance ();


// Create online change notifications for users who made changes in Bibles through the web editor.
// It runs before the team changes.
// This produces the desired order of the notifications in the GUI.
$users = $database_modifications->getUserUsernames ();
if (!empty ($users)) $database_logs->log ("Generating lists of changes made per user", Filter_Roles::TRANSLATOR_LEVEL);
foreach ($users as $user) {

  // Go through the Bibles changed by the current user.
  $bibles = $database_modifications->getUserBibles ($user);
  foreach ($bibles as $bible) {

    // Body of the email to be sent.
    $email = "<p>" . Locale_Translate::_("You have entered the changes below in the online Bible Editor.") ." " . Locale_Translate::_ ("You may check if it made its way into the Bible text.") . "</p>";

    // Go through the books in that Bible.
    $books = $database_modifications->getUserBooks ($user, $bible);
    foreach ($books as $book) {

      // Go through the chapters in that book.
      $chapters = $database_modifications->getUserChapters ($user, $bible, $book);
      foreach ($chapters as $chapter) {

        // Get the sets of identifiers for that chapter, and set some variables.
        $IdSets = $database_modifications->getUserIdentifiers ($user, $bible, $book, $chapter);
        $referenceOldId = 0;
        $referenceNewId = 0;
        $lastNewId = 0;
        $restart = true;

        // Go through the sets of identifiers.
        foreach ($IdSets as $IdSet) {

          $oldId = $IdSet ['oldid'];
          $newId = $IdSet ['newid'];

          if ($restart) {
            processIdentifiers ($user, $bible, $book, $chapter, $referenceNewId, $newId, $email);
            $referenceOldId = $oldId;
            $referenceNewId = $newId;
            $lastNewId = $newId;
            $restart = false;
            continue;
          }

          if ($oldId == $lastNewId) {
            $lastNewId = $newId;
          } else {
            $restart = true;
          }

        }

        // Process the last set of identifiers.
        processIdentifiers ($user, $bible, $book, $chapter, $referenceNewId, $newId, $email);

      }
    }

    // Send the user email with the user's personal changes if the user opted to receive it.
    if ($database_config_user->getUserUserChangesNotification ($user)) {
      $subject = Locale_Translate::_("Changes you entered in") . " " . $bible;
      if (!Filter_Client::enabled ()) $database_mail->send ($user, $subject, $email);
    }
    unset ($email);

  }

  // Clear the user's changes in the database.
  $database_modifications->clearUserUser ($user);

}


// Helper function.
function processIdentifiers ($user, $bible, $book, $chapter, $oldId, $newId, &$email)
{
  if ($oldId != 0) {
    $database_modifications = Database_Modifications::getInstance ();
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_bible = Database_Config_Bible::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_history = Database_History::getInstance ();
    $stylesheet = $database_config_bible->getExportStylesheet ($bible);
    $old_chapter_usfm = $database_modifications->getUserChapter ($user, $bible, $book, $chapter, $oldId);
    $old_chapter_usfm = $old_chapter_usfm ['oldtext'];
    $new_chapter_usfm = $database_modifications->getUserChapter ($user, $bible, $book, $chapter, $newId);
    $new_chapter_usfm = $new_chapter_usfm ['newtext'];
    $timestamp = $database_modifications->getUserTimestamp ($user, $bible, $book, $chapter, $newId);
    $old_verse_numbers = Filter_Usfm::getVerseNumbers ($old_chapter_usfm);
    $new_verse_numbers = Filter_Usfm::getVerseNumbers ($new_chapter_usfm);
    $verses = array_merge ($old_verse_numbers, $new_verse_numbers);
    $verses = array_unique ($verses);
    sort ($verses, SORT_NUMERIC);
    foreach ($verses as $verse) {
      $old_verse_usfm = Filter_Usfm::getVerseText ($old_chapter_usfm, $verse);
      $new_verse_usfm = Filter_Usfm::getVerseText ($new_chapter_usfm, $verse);
      if ($old_verse_usfm != $new_verse_usfm) {
        $filter_text_old = new Filter_Text ($bible);
        $filter_text_new = new Filter_Text ($bible);
        $filter_text_old->html_text_standard = new Html_Text (Locale_Translate::_("Bible"));
        $filter_text_new->html_text_standard = new Html_Text (Locale_Translate::_("Bible"));
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
        if ($old_text != $new_text) {
          $modification = Filter_Diff::diff ($old_text, $new_text);
          $email .= "<div>";
          $email .= Filter_Books::passageDisplay ($book, $chapter, $verse);
          $email .= " ";
          $email .= $modification;
          $email .= "</div>";
          if ($database_config_user->getUserUserChangesNotificationsOnline ($user)) {
            $changeNotificationUsers = array ($user);
            $database_modifications->recordNotification ($changeNotificationUsers, "☺", $bible, $book, $chapter, $verse, $old_html, $modification, $new_html);
          }
          $database_history->record ($timestamp, $user, $bible, $book, $chapter, $verse, $old_html, $modification, $new_html);
        }
      }
    }
  }
}


// Generate the notifications, online and by email, 
// for the changes in the Bibles accepted by the team since the previous notifications were generated.
$bibles = $database_modifications->getTeamDiffBibles ();
foreach ($bibles as $bible) {


  $stylesheet = $database_config_bible->getExportStylesheet ($bible);


  $changeNotificationUsers = array ();
  $users = $database_users->getUsers ();
  foreach ($users as $user) {
    if (Access_Bible::read ($bible, $user)) {
      if ($database_config_user->getUserGenerateChangeNotifications ($user)) {
        $changeNotificationUsers [] = $user;
      }
    }
  }
  unset ($users);


  // The number of changes processed so far for this Bible.
  $processedChangesCount = 0;
  

  // The files get stored at http://site.org/bibledit-web/changes/<Bible>/<date>
  $basePath  = "revisions/" . $bible . "/" . strftime ("%Y-%m-%d_%H:%M:%S");
  $directory = realpath ("..") . "/$basePath";
  mkdir ($directory, 0777, true);


  // Produce the USFM and html files.
  Filter_Diff::produceVerseLevel ($bible, $directory);


  // Create online page with changed verses.
  $versesoutputfile = "$directory/changed_verses.html";
  Filter_Diff::runDiffFile ("$directory/verses_old.txt", "$directory/verses_new.txt", $versesoutputfile);


  // Email users.
  $subject = Locale_Translate::_("Recent changes") . " " . $bible;
  $emailBody = file_get_contents ($versesoutputfile);
  $users = $database_users->getUsers ();
  foreach ($users as $user) {
    if ($database_config_user->getUserBibleChangesNotification ($user)) {
      if (Access_Bible::read ($bible, $user)) {
        if (!Filter_Client::enabled ()) $database_mail->send ($user, $subject, $emailBody);
      }
    }
  }


  // Generate the online change notifications.
  $books = $database_modifications->getTeamDiffBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_modifications->getTeamDiffChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $database_logs->log ("$bible " . Filter_Books::passageDisplay ($book, $chapter, "") . " Listing changes", Filter_Roles::TRANSLATOR_LEVEL);
      $old_chapter_usfm = $database_modifications->getTeamDiff ($bible, $book, $chapter);
      $new_chapter_usfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $old_verse_numbers = Filter_Usfm::getVerseNumbers ($old_chapter_usfm);
      $new_verse_numbers = Filter_Usfm::getVerseNumbers ($new_chapter_usfm);
      $verses = array_merge ($old_verse_numbers, $new_verse_numbers);
      $verses = array_unique ($verses);
      sort ($verses, SORT_NUMERIC);
      foreach ($verses as $verse) {
        $old_verse_usfm = Filter_Usfm::getVerseText ($old_chapter_usfm, $verse);
        $new_verse_usfm = Filter_Usfm::getVerseText ($new_chapter_usfm, $verse);
        if ($old_verse_usfm != $new_verse_usfm) {
          $processedChangesCount++;
          // In case of too many change notifications, processing them would take too much time, so take a few shortcuts.
          if ($processedChangesCount < 800) {
            $filter_text_old = new Filter_Text ($bible);
            $filter_text_new = new Filter_Text ($bible);
            $filter_text_old->html_text_standard = new Html_Text (Locale_Translate::_("Bible"));
            $filter_text_new->html_text_standard = new Html_Text (Locale_Translate::_("Bible"));
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
          } else {
            $old_html = "<p>$old_verse_usfm</p>";
            $new_html = "<p>$new_verse_usfm</p>";
            $old_text = $old_verse_usfm;
            $new_text = $new_verse_usfm;
          }
          if ($old_text != $new_text) {
            $modification = Filter_Diff::diff ($old_text, $new_text);
            $database_modifications->recordNotification ($changeNotificationUsers, "♺", $bible, $book, $chapter, $verse, $old_html, $modification, $new_html);
          }
        }
      }
      // Delete the diff data for this chapter, for two reasons:
      // 1. New diffs for this chapter can be stored straightaway.
      // 2. In case of large amounts of diff data, and this script gets killed, 
      //    then the next time this script runs again, it will continue from where the previous script was killed.
      $database_modifications->deleteTeamDiffChapter ($bible, $book, $chapter);
    }
  }


}


// Index the data and remove expired notifications.
$database_logs->log ("Indexing the online change notifications", Filter_Roles::TRANSLATOR_LEVEL);
$database_modifications->indexTrimAllNotifications ();


$database_logs->log ("Ready generating lists of changes in the Bibles", Filter_Roles::TRANSLATOR_LEVEL);


?>
