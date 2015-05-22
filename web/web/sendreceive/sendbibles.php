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
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_bibleactions = Database_BibleActions::getInstance ();
$database_users = Database_Users::getInstance ();
$database_books = Database_Books::getInstance ();


$database_logs->log (Locale_Translate::_("Sending and receiving Bibles"), Filter_Roles::TRANSLATOR_LEVEL);


$response = Filter_Client::setup ();
if ($response === false || $response < Filter_Roles::GUEST_LEVEL || $response > Filter_Roles::ADMIN_LEVEL) {
  $database_logs->log (Locale_Translate::_("Failure initializing sending and receiving Bibles"), Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


$address = $database_config_general->getServerAddress ();


$users = $database_users->getUsers ();
$user = $users [0];
$hash = $database_users->getmd5 ($user);


$communication_errors = false;


// Go through the Bibles / books / chapters that have actions recorded for them.
$bibles = $database_bibleactions->getBibles ();
foreach ($bibles as $bible) {
  $books = $database_bibleactions->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibleactions->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {

      $bookname = $database_books->getEnglishFromId ($book);
      $database_logs->log (Locale_Translate::_("Sending to server") . ": $bible $bookname $chapter", Filter_Roles::TRANSLATOR_LEVEL);

      // Get old and new USFM for this chapter.
      $oldusfm = $database_bibleactions->getUsfm ($bible, $book, $chapter);
      $newusfm = $database_bibles->getChapter ($bible, $book, $chapter);
      
      // Straightaway clear the Bible action for this chapter.
      // This atomic operation enables new edits from the user in this chapter to be recorded straightaway, 
      // even during the time that this chapter is still being sent.
      // In the face of a slow network at times, this does occur in practise.
      // Examples have been seen.
      $database_bibleactions->delete ($bible, $book, $chapter);

      // If old USFM and new USFM differ, and the new USFM is not empty, send it to the server.
      if (($newusfm != $oldusfm) && ($newusfm != "")) {

        $checksum = Checksum_Logic::get ($oldusfm . $newusfm);

        // It used to send the old and new USFM in compressed form.
        // But the server failed to decompress it.
        // That means that compression is not a good option.
        
        // Generate a POST request.
        $post = array (
          "u"  => bin2hex ($user), 
          "p"  => $hash,
          "bi" => $bible,
          "bo" => $book,
          "ch" => $chapter,
          "o"  => $oldusfm,
          "n"  => $newusfm,
          "s"  => $checksum
        );
        
        $url = "$address/sync/bible.php";
        $response = Sync_Logic::post ($post, $url);
    
        if ($response === false) {

          // Communication error.
          $communication_errors = true;
          $database_logs->log ("Failure sending Bibles", Filter_Roles::TRANSLATOR_LEVEL);
          // Restore the Bible action for this chapter.
          $database_bibleactions->delete ($bible, $book, $chapter);
          $database_bibleactions->record ($bible, $book, $chapter, $oldusfm);

        }

        else if ($response != "") {

          // The first line of the response can contain a message as a string.
          // This is what the server says to the client in case of an error.
          // Normally the first line of the response contains a numerical value.
          // This value is the checksum of the data that the server returns in the subsequent lines.
          // This data would be the updated USFM for the chapter.
          $response = explode ("\n", $response);
          $checksum_message = array_shift ($response);
          if (is_numeric ($checksum_message)) {
            
            $response = implode ("\n", $response);
            
            if (Checksum_Logic::get ($response) == $checksum_message) {
              Bible_Logic::storeChapter ($bible, $book, $chapter, $response);
            } else {
              // Checksum error.
              // The chapter will not be stored on the client.
              // That is fine for just now, because the pending sync action will do it.
              $database_logs->log ("Checksum error while receiving chapter from server", Filter_Roles::TRANSLATOR_LEVEL);
            }
  
          } else {
            $database_logs->log ("The server says: $checksum_message", Filter_Roles::TRANSLATOR_LEVEL);
          }

        } else {

          // If the server sends no response, 
          // then the server decided that there is no need to send the updated chapter back to the client,
          // as it considers the client as already up to date.

        }

      }

    }
  }
}


// Test the following:
// * Slight edits on client: Transfer to the server.
// * Edits on client and on server: Merge both versions.
// * Big edits on client: Transfer to server.
// * Empty data on server: The client version is stored on the server.
// * Client has no access: It displays message in the journal.
// * Merge conflict: Takes the server's copy, and the server emails the user.
// * Network latency: Test it by adding delays here and there.


// After all Bible actions have been sent to the server, and the chapters updated on the client,
// the client will now sync its Bibles with the server's Bibles.
// But if there was a communications error during sending a change to the server, 
// then the client should not download changes from the server,
// because downloading them would overwrite the changes on the client.
// The client only downloads changes from the server after it has sent all local edits successfully.
if (!$communication_errors) Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/syncbibles.php"));


?>
