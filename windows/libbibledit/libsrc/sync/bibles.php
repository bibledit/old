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


$database_users = Database_Users::getInstance (); 
$database_logs = Database_Logs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();
$session_logic = Session_Logic::getInstance ();
$database_mail = Database_Mail::getInstance ();


@$username = Filter_Hex::hex2bin ($_POST ['u']);
@$password = $_POST ['p'];
@$bible = $_POST ['b'];
@$book = $_POST ['bk'];
@$chapter = $_POST ['c'];
$action = $_POST ['a'];


if ($action == "total") {


  // The server reads the credentials from the client's user,
  // checks which Bibles this user has access to,
  // calculate the checksum of all chapters in those Bibles,
  // and returns this checksum to the client.
  $user_ok = $database_users->usernameExists ($username);
  if (!$user_ok) $database_logs->log ("Non existing user $username", Filter_Roles::MANAGER_LEVEL);
  $pass_ok = ($password == $database_users->getmd5 ($username));
  if (!$pass_ok) $database_logs->log ("Incorrect password $password for user $username", Filter_Roles::MANAGER_LEVEL);
  if (!$user_ok || !$pass_ok) {
    // Unauthorized.
    http_response_code (401);
    die;
  }

  $bibles = Access_Bible::bibles ($username);
  $server_checksum = Checksum_Logic::getBibles ($bibles);
  echo $server_checksum;


} else if ($action == "bibles") {


  // The server reads the credentials from the client's user,
  // and responds with a list of Bibles this user has access to.
  if ($password != $database_users->getmd5 ($username)) {
    // Unauthorized.
    http_response_code (401);
    die;
  }

  $bibles = Access_Bible::bibles ($username);
  $bibles = implode ("\n", $bibles);
  $checksum = Checksum_Logic::get ($bibles);
  echo "$checksum\n$bibles";


} else if ($action == "bible") {


  // The server responds with the checksum for the whole Bible.
  $server_checksum = Checksum_Logic::getBible ($bible);
  echo $server_checksum;


} else if ($action == "books") {


  // The server responds with a checksum and then the list of books in the Bible.
  $server_books = $database_bibles->getBooks ($bible);
  $server_books = implode ("\n", $server_books);
  $checksum = Checksum_Logic::get ($server_books);
  echo "$checksum\n$server_books";


} else if ($action == "book") {


  // The server responds with the checksum of the whole book.
  $server_checksum = Checksum_Logic::getBook ($bible, $book);
  echo $server_checksum;


} else if ($action == "chapters") {


  // The server responds with the list of books in the Bible book.
  $server_chapters = $database_bibles->getChapters ($bible, $book);
  $server_chapters = implode ("\n", $server_chapters);
  $checksum = Checksum_Logic::get ($server_chapters);
  echo "$checksum\n$server_chapters";


} else if ($action == "chapter") {


  // The server responds with the checksum of the whole chapter.
  $server_checksum = Checksum_Logic::getChapter ($bible, $book, $chapter);
  echo $server_checksum;


} else if ($action == "get") {
  
  
  // The server responds with the USFM of the chapter, prefixed by a checksum.
  $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
  $checksum = Checksum_Logic::get ($usfm);
  echo "$checksum\n$usfm";
  
  
}


?>
