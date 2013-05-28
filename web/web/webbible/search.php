<?php

/*

Copyright (©) 2012-2013 Teus Benschop.

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
page_access_level (GUEST_LEVEL);


echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\" >\n";
echo "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
echo "<head>\n";
echo "  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"></meta>\n";
echo "  <title>Search</title>\n";
echo "  <link rel=\"stylesheet\" type=\"text/css\" href=\"stylesheet.css\"></link>\n";
echo "</head>\n";
echo "<body>\n";


// Get the URL and the text for the backlink.
$backlinkUrl = isset($_GET['url']) ? $_GET['url'] : '';
$backlinkText = isset($_GET['text']) ? $_GET['text'] : '';


// The query: The word or string to search for.
$queryString = isset($_GET['q']) ? $_GET['q'] : '';


// Write the search box.
// Put the search query and the backlink in it.
echo "<table width=\"100%\">\n";
echo "<tr>\n";
echo "<td><a href=\"$backlinkUrl\" id=\"\" title=\"$backlinkText\">$backlinkText</a></td>\n";
echo "<td align=\"right\">\n";
echo "<form action=\"search.php\" method=\"GET\" name=\"search\" id=\"search\">\n";
echo "<input name=\"q\" type=\"text\" value=\"$queryString\">\n";
echo "<input type=\"image\" name=\"search\" src=\"lens.png\">\n";
echo "<input type=\"hidden\" name=\"url\" value=\"$backlinkUrl\">\n";
echo "<input type=\"hidden\" name=\"text\" value=\"$backlinkText\">\n";
echo "</form>\n";
echo "</td>\n";
echo "</tr>\n";
echo "</table>\n";


// Clean the query string up.
$queryString = trim ($queryString);


// Generate search words for emphasizing the search hits.
$queryWords = explode (" ", $queryString);
$markedWords = array ();
foreach ($queryWords as $word) {
  $markedWords [] = '<mark>' . $word . '</mark>';
}


// Bible databases.
$database_bibles = Database_Bibles::getInstance ();


// Exported Bible.
$exportedBible = basename (dirname (dirname ($backlinkUrl)));
$exportedID = $database_bibles->getID ($exportedBible);


// Search the Bible text.
$ids = $database_bibles->searchText ($queryString);


// Only take the results that related to the exported Bible.
$ids2 = array ();
foreach ($ids as $id) {
  $details = $database_bibles->getBibleBookChapter ($id);
  if ($exportedID == $details["bible"]) {
    $ids2 [] = $id;
  }
}
$ids = $ids2;


// Hit count.
$hitCount = count ($ids);
echo "<font size=\"-1\" color=\"grey\"><hr /></font>\n";
echo "<p><font size=\"-1\" color=\"grey\">$hitCount " . gettext ("chapters") . "</font></p>\n";


// Go through the search hits.
foreach ($ids as $id) {


  // Get the details of this search hit.
  $details = $database_bibles->getBibleBookChapter ($id);
  if ($details == NULL) continue;
  $bible = $database_bibles->getName ($details["bible"]);
  $book = $details ["book"];
  $chapter = $details ["chapter"];


  // The title.
  $title = "$bible" . " | " . Filter_Books::passageDisplay ($book, $chapter, "");
  $title = substr ($title, 0, -1);
  $title = Filter_Html::sanitize ($title);


  // The URL.
  $url = "../downloads/exports/$bible/web/" . Filter_Paths::htmlFileNameBible ("", $book, $chapter);


  // Output title and URL.
  echo "<p style=\"margin-top: 0.75em; margin-bottom: 0em\"><a href=\"$url\">$title</a></p>\n";


  // The excerpt.
  $text = $database_bibles->getSearchField ($id);
  $text = explode ("\n", $text);
  $excerpt = "";
  // Go through each line of text separately.
  foreach ($text as $line) {
    $count = 0;
    $line = str_ireplace ($queryWords, $markedWords, $line, $count);
    if ($count == 0) continue;
    // Store this bit of the excerpt.
    $excerpt .= "<p style=\"margin-top: 0em; margin-bottom: 0em\">$line</p>\n";
  }
  echo "$excerpt\n";


}


// Last bit of the page.
echo "</body>\n";


?>
