<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::consultant ());


$database_history = Database_History::getInstance ();
$session_logic = Session_Logic::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$username = request->session_logic()->currentUser ();
$focused_book = Ipc_Focus::getBook ();
$focused_chapter = Ipc_Focus::getChapter ();
$focused_verse = Ipc_Focus::getVerse ();


$header = new Assets_Header (gettext("Change History"));
$header->setNavigator ();
$header->setStylesheet ();
$header->run ();


$view = new Assets_View (__FILE__);


// Bits for the display state.
$state = array ();
// Where to start displaying items.
// It displays a limited number of items for performance reasons.
@$start = request->query ['start'];
if (!isset ($start)) $start = 0;
if (!is_numeric ($start)) $start = 0;
$state ['start'] = $start;
// Filter on Bible / book / chapter / verse.
@$passage = request->query ['passage'];
if (!isset ($passage)) $passage = 0;
if (!is_numeric ($passage)) $passage = 0;
if (isset ($passage)) $state ['passage'] = $passage;
// Filter on author.
// It filters on the offset of the array of authors fetched from the database.
@$author = request->query ['author'];
if (isset ($author)) $state ['author'] = $author;


// The Bibles the user has read access to.
$myBibles = access_bible_bibles ();


// The names of the authors in the history, with 'everybody' on top.
$author_names = $database_history->authors ($myBibles);
array_unshift ($author_names, gettext("everybody"));


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
$view.set_variable ("count = $count;


// Displaying items from $start to $end.
$end = $start + 50;
if ($end > $count) $end = $count;
if ($count == 0) {
  $start = -1;
  $end = 0;
}
$view.set_variable ("start = $start + 1;
$view.set_variable ("end = $end;


// The various bits of the history from the database.
$passageTexts = array ();
$authors = array ();
$dates = array ();
$bibles = array ();
$oldTexts = array ();
$modifications = array ();
$newTexts = array ();
$data = $database_history->get ($author_filter, $myBibles, $book_filter, $chapter_filter, $verse_filter, $start);
for ($data as $entry) {
  $passageText = filter_passage_display_inline (array (array ($entry['book'], $entry['chapter'], $entry['verse'])));
  $passageText = filter_string_sanitize_html ($passageText);
  $passageTexts [] = $passageText;
  $authors [] = filter_string_sanitize_html ($entry ['author']);
  $dates [] = $timestamp = date ('j F Y g:i:s a', $entry ['timestamp']);
  $bibles [] = filter_string_sanitize_html ($entry ['bible']);
  $oldTexts [] = $entry ['oldtext'];
  $modifications [] = $entry ['modification'];
  $newTexts [] = $entry ['newtext'];
}
$view.set_variable ("passageTexts = $passageTexts;
$view.set_variable ("authors = $authors;
$view.set_variable ("dates = $dates;
$view.set_variable ("bibles = $bibles;
$view.set_variable ("oldTexts = $oldTexts;
$view.set_variable ("modifications = $modifications;
$view.set_variable ("newTexts = $newTexts;


// Data for going back and forward in the pager.
$back = $start;
if ($back > 0) $back -= 50;
if ($back < 0) $back = 0;
$forward = $start + 50;
$view.set_variable ("back = http_build_query (array_merge ($state, array ('start' => $back)));
$view.set_variable ("forward = http_build_query (array_merge ($state, array ('start' => $forward)));


// Data for the passage filter.
$view.set_variable ("bible = http_build_query (array_merge ($state, array ('passage' => 0)));
$view.set_variable ("book = http_build_query (array_merge ($state, array ('passage' => 1)));
$view.set_variable ("chapter = http_build_query (array_merge ($state, array ('passage' => 2)));
$view.set_variable ("verse = http_build_query (array_merge ($state, array ('passage' => 3)));


// Data for the author filter.
$author_names = $database_history->authors ($myBibles);
array_unshift ($author_names, gettext("everybody"));
$view.set_variable ("author_names = $author_names;
$author_queries = array ();
for ($author_names as $offset => $dummy) {
  $author_queries [] = http_build_query (array_merge ($state, array ('author' => $offset)));
}
$view.set_variable ("author_queries = $author_queries;


$view->render ("history");


Assets_Page::footer ();


?>
