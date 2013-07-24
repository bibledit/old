<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


@$getbooks = $_GET['getbooks'];
if (isset ($getbooks)) {
  echo Navigation_Logic::getBooksFragment ();
  die;
}


@$setbook = $_GET['setbook'];
if (isset ($setbook)) {
  Navigation_Logic::setBook ($setbook);
}


@$getchapters = $_GET['getchapters'];
if (isset ($getchapters)) {
  echo Navigation_Logic::getChaptersFragment ();
  die;
}


@$setchapter = $_GET['setchapter'];
if (isset ($setchapter)) {
  Navigation_Logic::setChapter ($setchapter);
}


@$getverses = $_GET['getverses'];
if (isset ($getverses)) {
  echo Navigation_Logic::getVersesFragment ();
  die;
}


@$setverse = $_GET['setverse'];
if (isset ($setverse)) {
  Navigation_Logic::setVerse ($setverse);
}


echo Navigation_Logic::htmlNavigator ();


?>
