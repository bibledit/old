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


$database_history = Database_History::getInstance ();
$session_logic = Session_Logic::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$username = $session_logic->currentUser ();
$focused_book = $ipc_focus->getBook ();
$focused_chapter = $ipc_focus->getChapter ();
$focused_verse = $ipc_focus->getVerse ();


$header = new Assets_Header (Locale_Translate::_("Change History"));
$header->setNavigator ();
$header->setStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);


// Bits for the display state.
$state = array ();
// Where to start displaying items.
// It displays a limited number of items for performance reasons.
@$start = $_GET ['start'];
if (!isset ($start)) $start = 0;
if (!is_numeric ($start)) $start = 0;
$state ['start'] = $start;
// Filter on Bible / book / chapter / verse.
@$passage = $_GET ['passage'];
if (!isset ($passage)) $passage = 0;
if (!is_numeric ($passage)) $passage = 0;
if (isset ($passage)) $state ['passage'] = $passage;
// Filter on author.
// It filters on the offset of the array of authors fetched from the database.
@$author = $_GET ['author'];
if (isset ($author)) $state ['author'] = $author;


// The Bibles the user has read access to.
$myBibles = Access_Bible::bibles ();


// The names of the authors in the history, with 'everybody' on top.
$author_names = $database_history->authors ($myBibles);
array_unshift ($author_names, Locale_Translate::_("everybody"));


// The filter variables.
$author_filter = NULL;
$book_filter = NULL;
$chapter_filter = NULL;
$verse_filter = NULL;
if (isset ($author) && ($author > 0)) {
  $author_filter = $author_names [$author];
}
switch ($passage) {
  case 3: $verse_filter = $focused_verse;
  case 2: $chapter_filter = $focused_chapter;
  case 1: $book_filter = $focused_book;
  case 0: break;
}


// Total item count.
$count = $database_history->count ($author_filter, $myBibles, $book_filter, $chapter_filter, $verse_filter);
$view->view->count = $count;


// Displaying items from $start to $end.
$end = $start + 50;
if ($end > $count) $end = $count;
if ($count == 0) {
  $start = -1;
  $end = 0;
}
$view->view->start = $start + 1;
$view->view->end = $end;


// The various bits of the history from the database.
$passageTexts = array ();
$authors = array ();
$dates = array ();
$bibles = array ();
$oldTexts = array ();
$modifications = array ();
$newTexts = array ();
$data = $database_history->get ($author_filter, $myBibles, $book_filter, $chapter_filter, $verse_filter, $start);
foreach ($data as $entry) {
  $passageText = Filter_Books::passagesDisplayInline (array (array ($entry['book'], $entry['chapter'], $entry['verse'])));
  $passageText = Filter_Html::sanitize ($passageText);
  $passageTexts [] = $passageText;
  $authors [] = Filter_Html::sanitize ($entry ['author']);
  $dates [] = $timestamp = date ('j F Y g:i:s a', $entry ['timestamp']);
  $bibles [] = Filter_Html::sanitize ($entry ['bible']);
  $oldTexts [] = $entry ['oldtext'];
  $modifications [] = $entry ['modification'];
  $newTexts [] = $entry ['newtext'];
}
$view->view->passageTexts = $passageTexts;
$view->view->authors = $authors;
$view->view->dates = $dates;
$view->view->bibles = $bibles;
$view->view->oldTexts = $oldTexts;
$view->view->modifications = $modifications;
$view->view->newTexts = $newTexts;


// Data for going back and forward in the pager.
$back = $start;
if ($back > 0) $back -= 50;
if ($back < 0) $back = 0;
$forward = $start + 50;
$view->view->back = http_build_query (array_merge ($state, array ('start' => $back)));
$view->view->forward = http_build_query (array_merge ($state, array ('start' => $forward)));


// Data for the passage filter.
$view->view->bible = http_build_query (array_merge ($state, array ('passage' => 0)));
$view->view->book = http_build_query (array_merge ($state, array ('passage' => 1)));
$view->view->chapter = http_build_query (array_merge ($state, array ('passage' => 2)));
$view->view->verse = http_build_query (array_merge ($state, array ('passage' => 3)));


// Data for the author filter.
$author_names = $database_history->authors ($myBibles);
array_unshift ($author_names, Locale_Translate::_("everybody"));
$view->view->author_names = $author_names;
$author_queries = array ();
foreach ($author_names as $offset => $dummy) {
  $author_queries [] = http_build_query (array_merge ($state, array ('author' => $offset)));
}
$view->view->author_queries = $author_queries;


$view->render ("history.php");


Assets_Page::footer ();


?>
