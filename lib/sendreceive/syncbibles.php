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


$address = $database_config_general->getServerAddress ();
$url = "$address/sync/bibles.php";


$users = $database_users->getUsers ();
$user = $users [0];
$hash = $database_users->getmd5 ($user);


// Calculate the total checksum of all chapters in all books in all local Bibles.
// Send the credentials to the server, 
// to enable the server to calculate a similar checksum for all Bibles on the server
// that the client username has access to via its credentials.
// The client compares the two checksums.
// If they match, it means everything is in sync.

$bibles = $database_bibles->getBibles ();
$client_checksum = Checksum_Logic::getBibles ($bibles);

$post = array (
  "u" => bin2hex ($user), 
  "p" => $hash,
  "a" => "total"
);

$server_checksum = Sync_Logic::post ($post, $url);

if ($server_checksum === false) {
  $database_logs->log (Locale_Translate::_("Failure getting Bibles checksum"), Filter_Roles::TRANSLATOR_LEVEL);
  die;
}

if ($client_checksum == $server_checksum) {
  $database_logs->log (Locale_Translate::_("The Bibles are up to date"), Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


// Send the user credentials to the server and request the Bibles.
// The server responds with the Bibles this user has access to.
// The client stores this list for later use.

$post = array (
  "u" => bin2hex ($user), 
  "p" => $hash,
  "a" => "bibles"
);

$server_bibles = Sync_Logic::post ($post, $url);

if ($server_bibles === false) {
  $database_logs->log (Locale_Translate::_("Failure getting list of Bibles"), Filter_Roles::TRANSLATOR_LEVEL);
  die;
}

// Verify the checksum of the list of Bibles to be sure that the data is valid.
// This is important, because there have been cases that the list of Bible was corrupted, 
// so the client deleted all its local Bibles.
// Checksumming prevents this behaviour.
$server_bibles = explode ("\n", $server_bibles);
$server_checksum = array_shift ($server_bibles);
$message_checksum = Checksum_Logic::get (implode ("n", $server_bibles));
if ($server_checksum != $message_checksum) {
  $database_logs->log (Locale_Translate::_("Checksum error while receiving list of Bibles from server:"), Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$database_logs->log (Locale_Translate::_("Bibles") . ": " . implode (", ", $server_bibles), Filter_Roles::TRANSLATOR_LEVEL);


// The client now has a list of Bibles the user has access to on the server.
// The client deletes all local Bibles not in this list.
// No change Bible actions to be recorded for this operation.
$bibles = $database_bibles->getBibles ();
$bibles = array_diff ($bibles, $server_bibles);
foreach ($bibles as $bible) {
  $database_bibles->deleteBible ($bible);
  $database_logs->log (Locale_Translate::_("Deleting Bible because the server did not grant access to it") . ": " . $bible, Filter_Roles::TRANSLATOR_LEVEL);
}


// The client goes through all the Bibles, and deals with each of them.
foreach ($server_bibles as $bible) {


  // Compare the checksum of the whole Bible on client and server to see if this Bible is in sync.
  $client_checksum = Checksum_Logic::getBible ($bible);
  $post = array (
    "b" => $bible,
    "a" => "bible"
  );
  $server_checksum = Sync_Logic::post ($post, $url);
  if ($server_checksum === false) {
    $database_logs->log (Locale_Translate::_("Failure getting Bible checksum"), Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  if ($client_checksum == $server_checksum) {
    $database_logs->log (Locale_Translate::_("The Bible is up to date") . ": $bible", Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }


  // Request all books in the $bible on the server.
  $client_books = $database_bibles->getBooks ($bible);
  $post = array (
    "b" => $bible,
    "a" => "books"
  );
  $server_books = Sync_Logic::post ($post, $url);
  if ($server_books === false) {
    $database_logs->log (Locale_Translate::_("Failure getting Bible books"), Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  // Do checksumming on the data to be sure the data is valid.
  // Invalid data may cause books to be added or deleted on the client.
  $server_books = explode ("\n", $server_books);
  $server_checksum = array_shift ($server_books);
  $message_checksum = Checksum_Logic::get (implode ("n", $server_books));
  if ($server_checksum != $message_checksum) {
    $database_logs->log (Locale_Translate::_("Checksum error while receiving list of books from server"), Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  // Any books not on the server, delete them from the client as well.
  $client_books = array_diff ($client_books, $server_books);
  foreach ($client_books as $book) {
    $database_bibles->deleteBook ($bible, $book);
    $book_name = $database_books->getEnglishFromId ($book);
    $database_logs->log (Locale_Translate::_("Deleting book because the server does not have it") . ": $bible $book_name" , Filter_Roles::TRANSLATOR_LEVEL);
  }
 

  // The client goes through all the books as on the server, and deals with each of them.
  foreach ($server_books as $book) {


    $book_name = $database_books->getEnglishFromId ($book);


    // Compare the checksum for the whole book on the client with the same on the server to see if this book is in sync.
    $client_checksum = Checksum_Logic::getBook ($bible, $book);
    $post = array (
      "b" => $bible,
      "bk" => $book,
      "a" => "book"
    );
    $server_checksum = Sync_Logic::post ($post, $url);
    if ($server_checksum === false) {
      $database_logs->log (Locale_Translate::_("Failure getting book checksum"), Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    if ($client_checksum == $server_checksum) {
      continue;
    }
    

    // The client requests all chapters per book from the server.
    $client_chapters = $database_bibles->getChapters ($bible, $book);
    $post = array (
      "b" => $bible,
      "bk" => $book,
      "a" => "chapters"
    );
    $server_chapters = Sync_Logic::post ($post, $url);
    if ($server_chapters === false) {
      $bookname = $database_books->getEnglishFromId ($book);
      $database_logs->log (Locale_Translate::_("Failure getting list of chapters:") . " $bible $bookname", Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    $server_chapters = explode ("\n", $server_chapters);
    // Do checksumming on the data to be sure the data is valid.
    // Invalid data may cause chapters to be added or deleted on the client.
    $server_checksum = array_shift ($server_chapters);
    $message_checksum = Checksum_Logic::get (implode ("n", $server_chapters));
    if ($server_checksum != $message_checksum) {
      $database_logs->log (Locale_Translate::_("Checksum error while receiving list of chapters from server"), Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    // The client removes any local chapters not on the server.
    $client_chapters = array_diff ($client_chapters, $server_chapters);
    foreach ($client_chapters as $chapter) {
      $database_bibles->deleteChapter ($bible, $book, $chapter);
      $database_logs->log (Locale_Translate::_("Deleting chapter because the server does not have it") . ": $bible $book_name $chapter" , Filter_Roles::TRANSLATOR_LEVEL);
    }


    // The client goes through all chapters in the book.
    // It compares the checksum of that chapter locally with the same checksum on the server.
    // If the checksums differ, then the client requests the updated chapter from the server and stores it locally.
    // As the user on the client may continue editing the Bible while the sync operations runs,
    // the edits made by the user should not be overwritten by downloading the newest chapter from the server,
    // because in this case the newest chapter is not on the server, but on the client.
    // The solution is this:
    // When the Bible actions database contains an action for this particular chapter,
    // it means that chapter is being edited, and therefore the new chapter from the server should be merged with what is on the client.
    foreach ($server_chapters as $chapter) {

      // Get checksum for the chapter on client and on server.
      // If both are the same, it means the USFM in both is the same, and we're done.
      $client_checksum = Checksum_Logic::getChapter ($bible, $book, $chapter);
      $post = array (
        "b" => $bible,
        "bk" => $book,
        "c" => $chapter,
        "a" => "chapter"
      );
      $server_checksum = Sync_Logic::post ($post, $url);
      if ($server_checksum === false) {
        $database_logs->log (Locale_Translate::_("Failure getting checksum:") . " $bible $book_name $chapter", Filter_Roles::TRANSLATOR_LEVEL);
        continue;
      }
      if ($client_checksum == $server_checksum) {
        continue;
      }

      // Different checksums: Get the USFM for the chapter as it is on the server.
      $database_logs->log (Locale_Translate::_("Getting chapter:") . " $bible $book_name $chapter", Filter_Roles::TRANSLATOR_LEVEL);
      $post = array (
        "b" => $bible,
        "bk" => $book,
        "c" => $chapter,
        "a" => "get"
      );
      $server_usfm = Sync_Logic::post ($post, $url);
      if ($server_usfm === false) {
        $database_logs->log (Locale_Translate::_("Failure getting chapter:") . " $bible $book_name $chapter", Filter_Roles::TRANSLATOR_LEVEL);
        continue;
      }

      // Verify the checksum of the chapter on the server, to be sure there's no corruption during transmission.
      $server_usfm = explode ("\n", $server_usfm);
      $checksum = array_shift ($server_usfm);
      $server_usfm = implode ("\n", $server_usfm);
      if (!Checksum_Logic::get ($server_usfm) == $checksum) {
        $database_logs->log (Locale_Translate::_("Checksum error while receiving chapter from server:") . " $bible $book_name $chapter", Filter_Roles::TRANSLATOR_LEVEL);
        continue;
      }
      
      // Check whether the user on the client has made changes in this chapter since the edits were sent to the server.
      // If there are none, then the client stores the chapter as it gets it from the server, and is done.
      $old_usfm = $database_bibleactions->getUsfm ($bible, $book, $chapter);
      if ($old_usfm == "") {
        $database_bibles->storeChapter ($bible, $book, $chapter, $server_usfm);
        continue;
      }
      
      // At this stage we're in a situation that there are changes on the client, and changes on the server.
      // Merge them.
      // It uses a 3-way merge, taking the chapter from the bible actions as the basis, and the other two (client and server) to merge together.
      // Store the merged data on the client.
      // It stores through the Bible Logic so the changes are staged to be sent.
      // The changes will be sent to the server during the next synchronize action.
      $database_logs->log (Locale_Translate::_("Merging changes on server and client:") . " $bible $book_name $chapter", Filter_Roles::TRANSLATOR_LEVEL);
      $client_usfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $merged_usfm = Filter_Merge::run ($old_usfm, $client_usfm, $server_usfm);
      Bible_Logic::storeChapter ($bible, $book, $chapter, $merged_usfm);

    }


  }


}


?>
