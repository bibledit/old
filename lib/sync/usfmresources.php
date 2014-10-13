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


// Processing things may take time.
// Set the maximum execution time to indefinite.
set_time_limit (300);
ini_set ('max_execution_time', 300);


$database_logs = Database_Logs::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();


$action = $_POST ['a'];
@$resource = $_POST ['r'];
@$book = $_POST ['b'];
@$chapter = $_POST ['c'];


if ($action == "total") {

  $checksum = Sync_Logic::usfm_resources_checksum ();
  $checksum = serialize ($checksum);
  echo $checksum;

} else if ($action == "resources") {
  
  $resources = $database_usfmresources->getResources ();
  $resources = serialize ($resources);
  echo $resources;
  
} else if ($action == "resource") {

  $checksum = Sync_Logic::usfm_resource_checksum ($resource);
  $checksum = serialize ($checksum);
  echo $checksum;
  
} else if ($action == "books") {
  
  $books = $database_usfmresources->getBooks ($resource);
  $books = serialize ($books);
  echo $books;
  
} else if ($action == "book") {

  $checksum = Sync_Logic::usfm_resource_book_checksum ($resource, $book);
  $checksum = serialize ($checksum);
  echo $checksum;

} else if ($action == "chapters") {
  
  $chapters = $database_usfmresources->getChapters ($resource, $book);
  $chapters = serialize ($chapters);
  echo $chapters;
  
} else if ($action == "chapter") {

  $checksum = Sync_Logic::usfm_resource_chapter_checksum ($resource, $book, $chapter);
  $checksum = serialize ($checksum);
  echo $checksum;

} else if ($action == "get") {

  $contents = $database_usfmresources->getUsfm ($resource, $book, $chapter);
  $contents = serialize ($contents);
  echo $contents;

}


?>
