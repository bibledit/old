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


$database_config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();


$database_logs->log (Locale_Translate::_("Synchronizing USFM resources"), Filter_Roles::TRANSLATOR_LEVEL);


$address = $database_config_general->getServerAddress ();
$url = "$address/sync/usfmresources.php";


// Request the checksum of all USFM resources from the server.
// Compare it with the local checksum.
// If the two match: Ready.
$post = array (
  "a" => "total",
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure requesting totals for the USFM resources", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$checksum = Sync_Logic::usfm_resources_checksum ();
if ($response == $checksum) {
  $database_logs->log ("The USFM resources are up to date", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


// Request a list of all USFM resources available on the server.
$post = array (
  "a" => "resources",
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure requesting available USFM resources", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$server_resources = $response;


// Delete any resource on the local client but not on the server.
$client_resources = $database_usfmresources->getResources ();
$resources = array_diff ($client_resources, $server_resources);
foreach ($resources as $resource) {
  $database_usfmresources->deleteResource ($resource);
}


// Deal with each USFM resource individually.
foreach ($server_resources as $resource) {


  // Request the checksum of the resources from the server.
  // Compare it with the checksum of the local resource.
  // If they match: Go to the next resource.
  $post = array (
    "a" => "resource",
    "r" => $resource
  );
  $response = Sync_Logic::post ($post, $url);
  @$response = unserialize ($response);
  if ($response === false) {
    $database_logs->log ("Failure requesting checksum of USFM resource", Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  $checksum = Sync_Logic::usfm_resource_checksum ($resource);
  if ($response == $checksum) {
    continue;
  }


  // Request a list of all books in the resource on the server.
  $post = array (
    "a" => "books",
    "r" => $resource
  );
  $response = Sync_Logic::post ($post, $url);
  @$response = unserialize ($response);
  if ($response === false) {
    $database_logs->log ("Failure requesting books of USFM resource", Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  $server_books = $response;

  
  // Delete any books from the client that are not on the server.
  $client_books = $database_usfmresources->getBooks ($resource);
  $books = array_diff ($client_books, $server_books);
  foreach ($books as $book) {
    $database_usfmresources->deleteBook ($resource, $book);
  }
  
  
  // Deal with each book individually.
  foreach ($server_books as $book) {


    // Request checksum of this book,
    // compare it with the local checksum,
    // and skip the book if the checksums match.
    $post = array (
      "a" => "book",
      "r" => $resource,
      "b" => $book
    );
    $response = Sync_Logic::post ($post, $url);
    @$response = unserialize ($response);
    if ($response === false) {
      $database_logs->log ("Failure requesting checksum of USFM resource book", Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    $checksum = Sync_Logic::usfm_resource_book_checksum ($resource, $book);
    if ($response == $checksum) {
      continue;
    }


    $database_logs->log ("Synchronizing USFM resource" . " $resource $book", Filter_Roles::TRANSLATOR_LEVEL);


    // Retrieve a list of chapters in the $book from the server.
    $post = array (
      "a" => "chapters",
      "r" => $resource,
      "b" => $book
    );
    $response = Sync_Logic::post ($post, $url);
    @$response = unserialize ($response);
    if ($response === false) {
      $database_logs->log ("Failure requesting chapters of USFM resource book", Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    $server_chapters = $response;
    
    
    // Delete local chapters not found on the server.
    $client_chapters = $database_usfmresources->getChapters ($resource, $book);
    $chapters = array_diff ($client_chapters, $server_chapters);
    foreach ($chapters as $chapter) {
      $database_usfmresources->deleteChapter ($resource, $book, $chapter);
    }
    
    
    // Go through each chapter individually.
    foreach ($server_chapters as $chapter) {
      
      
      // Get the checksum of the chapter as it is on the server.
      $post = array (
        "a" => "chapter",
        "r" => $resource,
        "b" => $book,
        "c" => $chapter
      );
      $response = Sync_Logic::post ($post, $url);
      @$response = unserialize ($response);
      if ($response === false) {
        $database_logs->log ("Failure requesting checksum of USFM resource chapter", Filter_Roles::TRANSLATOR_LEVEL);
        continue;
      }
      $checksum = Sync_Logic::usfm_resource_chapter_checksum ($resource, $book, $chapter);
      if ($response == $checksum) {
        continue;
      }
      
      
      // Download the chapter from the server, and store it locally on the client.
      $post = array (
        "a" => "get",
        "r" => $resource,
        "b" => $book,
        "c" => $chapter
      );
      $response = Sync_Logic::post ($post, $url);
      @$response = unserialize ($response);
      if ($response === false) {
        $database_logs->log ("Failure downloading chapter of USFM resource", Filter_Roles::TRANSLATOR_LEVEL);
        continue;
      }
      $database_usfmresources->storeChapter ($resource, $book, $chapter, $response);


    }
  }
}


// Done.
$database_logs->log ("The USFM resources are now up to date", Filter_Roles::TRANSLATOR_LEVEL);


?>
