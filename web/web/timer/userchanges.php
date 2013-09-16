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


require_once ("../bootstrap/bootstrap.php");


$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("userchanges: Generating lists of changes in the Bibles per user"));


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("userchanges: Fatal: This only runs through the cli Server API");
  die;
}


$database_config_user = Database_Config_User::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_changesuser = Database_ChangesUser::getInstance ();


// Go through the users who have made changes in the Biblethrough the web editor.
$users = $database_changesuser->getUsernames ();
foreach ($users as $user) {

  // Check whether the user receives a notification.
  if ($database_config_user->getUserUserChangesNotification ($user)) {

    // Go through the Bibles changed by the current user.
    $bibles = $database_changesuser->getBibles ($user);
    foreach ($bibles as $bible) {
      
      // Body of the email to be sent.
      $emailbody = "";
        
      // Go through the books in that Bible.
      $books = $database_changesuser->getBooks ($user, $bible);
      foreach ($books as $book) {
        
        // Go through the chapters in that book.
        $chapters = $database_changesuser->getChapters ($user, $bible, $book);
        foreach ($chapters as $chapter) {
          
          // Get the sets of identifiers for that chapter, and set some variables.
          $IdSets = $database_changesuser->getIdentifiers ($user, $bible, $book, $chapter);
          $referenceOldId = 0;
          $referenceNewId = 0;
          $lastNewId = 0;
          $restart = true;

          // Go through the sets of identifiers.
          foreach ($IdSets as $IdSet) {
            
            $oldId = $IdSet ['oldid'];
            $newId = $IdSet ['newid'];

            if ($restart) {
              $emailbody .= processIdentifiers ($user, $bible, $book, $chapter, $referenceNewId, $newId);
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
          $emailbody .= processIdentifiers ($user, $bible, $book, $chapter, $referenceNewId, $newId);

        }
      }

      // Send the email.
      $subject = gettext ("Changes you entered in") . " " . $bible;
      $database_mail->send ($user, $subject, $emailbody);

    }
  }

  // Clear the user's changes in the database.
  $database_changesuser->clearUser ($user);

}


$database_logs->log (gettext ("userchanges: Completed"));


function processIdentifiers ($user, $bible, $book, $chapter, $oldId, $newId)
{
  $body = "";
  if ($oldId != 0) {
    $database_changesuser = Database_ChangesUser::getInstance ();
    $database_config_general = Database_Config_General::getInstance ();
    $stylesheet = $database_config_general->getExportStylesheet ();
    $old_chapter_usfm = $database_changesuser->getChapter ($user, $bible, $book, $chapter, $oldId);
    $old_chapter_usfm = $old_chapter_usfm ['oldtext'];
    $new_chapter_usfm = $database_changesuser->getChapter ($user, $bible, $book, $chapter, $newId);
    $new_chapter_usfm = $new_chapter_usfm ['newtext'];
    $old_verse_numbers = Filter_Usfm::getVerseNumbers ($old_chapter_usfm);
    $new_verse_numbers = Filter_Usfm::getVerseNumbers ($new_chapter_usfm);
    $verses = array_merge ($old_verse_numbers, $new_verse_numbers);
    $verses = array_unique ($verses);
    sort ($verses, SORT_NUMERIC);
    foreach ($verses as $verse) {
      $old_verse_usfm = Filter_Usfm::getVerseText ($old_chapter_usfm, $verse);
      $new_verse_usfm = Filter_Usfm::getVerseText ($new_chapter_usfm, $verse);
      if ($old_verse_usfm != $new_verse_usfm) {
        $filter_text_old = new Filter_Text ("");
        $filter_text_new = new Filter_Text ("");
        $filter_text_old->text_text = new Text_Text ();
        $filter_text_new->text_text = new Text_Text ();
        $filter_text_old->addUsfmCode ($old_verse_usfm);
        $filter_text_new->addUsfmCode ($new_verse_usfm);
        $filter_text_old->run ($stylesheet);
        $filter_text_new->run ($stylesheet);
        $old_text = $filter_text_old->text_text->get ();
        $new_text = $filter_text_new->text_text->get ();
        if ($old_text != $new_text) {
          $modification = Filter_Diff::diff ($old_text, $new_text);
          $body .= "<div>";
          $body .= Filter_Books::passageDisplay ($book, $chapter, $verse);
          $body .= " ";
          $body .= $modification;
          $body .= "</div>";
        }
      }
    }
  }
  return $body;
}


?>
