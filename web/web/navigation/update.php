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
page_access_level (CONSULTANT_LEVEL);


// The navigator does not fetch the active Bible from the user configuration,
// but from the query generated in the webbrowser by Javascript.
// The reason is that the Navigator can be independent of any Bible.
// If there's no Bible, the Navigator has all the 66 canonical books, plus their chapters and verses.
// If the Navigator has a Bible, it uses the books / chapters / verses from that Bible.
@$bible = $_GET['bible'];


@$goback = $_GET['goback'];
if (isset ($goback)) {
  Navigation_Logic::goBack ();
}


@$goforward = $_GET['goforward'];
if (isset ($goforward)) {
  Navigation_Logic::goForward ();
}


@$getbibles = $_GET['getbibles'];
if (isset ($getbibles)) {
  echo Navigation_Logic::getBiblesFragment ($bible);
  die;
}


@$setbible = $_GET['setbible'];
if (isset ($setbible)) {
  $database_config_user = Database_Config_User::getInstance ();
  $database_config_user->setBible ($bible);
}


@$getbooks = $_GET['getbooks'];
if (isset ($getbooks)) {
  echo Navigation_Logic::getBooksFragment ($bible);
  die;
}


@$setbook = $_GET['setbook'];
if (isset ($setbook)) {
  Navigation_Logic::setBook ($setbook);
}


@$getchapters = $_GET['getchapters'];
if (isset ($getchapters)) {
  echo Navigation_Logic::getChaptersFragment ($bible);
  die;
}


@$setchapter = $_GET['setchapter'];
if (isset ($setchapter)) {
  Navigation_Logic::setChapter ($setchapter);
}


@$getverses = $_GET['getverses'];
if (isset ($getverses)) {
  echo Navigation_Logic::getVersesFragment ($bible);
  die;
}


@$setverse = $_GET['setverse'];
if (isset ($setverse)) {
  Navigation_Logic::setVerse ($setverse);
}


@$getentry = $_GET['getentry'];
if (isset ($getentry)) {
  echo Navigation_Logic::getEntry ();
  die;
}


@$passage = $_GET['passage'];
if (isset ($passage)) {
  Navigation_Logic::setPassage ($bible, $passage);
  die;
}


echo Navigation_Logic::getNavigator ($bible);


?>
