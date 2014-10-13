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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


@$bible = $_GET ['bible'];
if (!isset ($bible)) {
  $database_config_user = Database_Config_User::getInstance ();
  $bible = $database_config_user->getBible ();
}


@$goback = $_GET['goback'];
if (isset ($goback)) {
  Navigation_Passage::goBack ();
}


@$goforward = $_GET['goforward'];
if (isset ($goforward)) {
  Navigation_Passage::goForward ();
}


@$getchapters = $_GET['getchapters'];
if (isset ($getchapters)) {
  $book = $_GET ['book'];
  $database_books = Database_Books::getInstance ();
  $book = $database_books->getIdFromEnglish ($book);
  echo Navigation_Passage::getChaptersFragment ($bible, $book, -1);
  die;
}


@$getverses = $_GET['getverses'];
if (isset ($getverses)) {
  $book = $_GET ['book'];
  if (!is_numeric ($book)) {
    $database_books = Database_Books::getInstance ();
    $book = $database_books->getIdFromEnglish ($book);
  }
  $chapter = $_GET ['chapter'];
  echo Navigation_Passage::getVersesFragment ($bible, $book, $chapter, -1);
  die;
}


@$verse = $_GET['verse'];
if (isset ($verse)) {
  $book = $_GET ['book'];
  if (!is_numeric ($book)) {
    $database_books = Database_Books::getInstance ();
    $book = $database_books->getIdFromEnglish ($book);
  }
  $chapter = $_GET ['chapter'];
  Navigation_Passage::setBookChapterVerse ($book, $chapter, $verse);
  die;
}


@$previousverse = $_GET['previousverse'];
if (isset ($previousverse)) {
  Navigation_Passage::gotoPreviousVerse ($bible);
}


@$nextverse = $_GET['nextverse'];
if (isset ($nextverse)) {
  Navigation_Passage::gotoNextVerse ($bible);
}


@$passage = $_GET['passage'];
if (isset ($passage)) {
  Navigation_Passage::setPassage ($bible, $passage);
  die;
}


echo Navigation_Passage::getNavigator ($bible);


?>
