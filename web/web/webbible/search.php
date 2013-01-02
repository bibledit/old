<?php

/*

Copyright (©) 2012-2012 Teus Benschop.

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

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\" >\n";
echo "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
echo "<head>\n";
echo "  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"></meta>\n";
echo "  <title>Search</title>\n";
echo "  <link rel=\"stylesheet\" type=\"text/css\" href=\"stylesheet.css\"></link>\n";
echo "</head>\n";
echo "<body>\n";


// The query: The word or string to search for.
$queryString = isset($_GET['q'])?$_GET['q']:'';


// Put the query string into the search box. Todo add this search box.


// Clean the query string up.
$queryString = trim ($queryString);
while (strpos ($queryString, "  ") !== false) {
  $queryString = str_replace ("  ", " ", $queryString);
}


// Add wildcards to the words in the query string.
$queryString = explode (" ", $queryString);
foreach ($queryString as &$line) {
  if ($line == "") continue;
  $line = "*" . $line . "*";
}
$queryString = implode (" ", $queryString);


require ("sphinxapi.php");


$sphinxClient = new SphinxClient ();
$sphinxClient->SetServer ("localhost", 9312);
$sphinxClient->SetConnectTimeout (1);
$sphinxClient->SetArrayResult (true);
$sphinxClient->SetMatchMode (SPH_MATCH_EXTENDED2);
$sphinxClient->SetLimits (0, 10000);


$queryResult = $sphinxClient->Query ($queryString, "sphinxsearch");
if ($queryResult === false) {
  echo "<p>Search failed: " . $sphinxClient->GetLastError () . "</p>\n";
} else {
  

  if ($sphinxClient->GetLastWarning()) {
    echo "<p>Warning: " . $sphinxClient->GetLastWarning () . "</p>\n";
  }


  // Number of search results. Todo test this.
  $totalFound = $queryResult['total_found'];
  if ($queryString == "") $totalFound = 0;
  echo "<font size=\"-1\" color=\"grey\"><hr /></font>\n";
  echo "<p><font size=\"-1\" color=\"grey\">$totalFound " . gettext ("results") . "</font></p>\n";


  // Assemble the search results.
  if ($totalFound > 0) {
    foreach ($queryResult["matches"] as $docinfo) {

      // The title.
      $title = $docinfo['attrs']['title'];
      $title = htmlspecialchars ($title, ENT_QUOTES, "UTF-8");

      // The url.
      $url = $docinfo['attrs']['url'];

      // The excerpt with the hits in bold.
      $text = $docinfo['attrs']['text'];
      $options = array
      (
        "before_match" => "<b>",
        "after_match" => "</b>",
        "chunk_separator" => " ... ",
        "limit" => 60,
        "around" => 3,
        "exact_phrase" => 0
      );
      // Go through each line of text separately.
      $text = explode ("\n", $text);
      $excerpt = "";
      foreach ($text as $textLine) {
        $partialExcerpt = $sphinxClient->BuildExcerpts (array ($textLine), "sphinxsearch", $queryString, $options);
        $partialExcerpt = $partialExcerpt [0];
        // If this line of text has no hit, skip it.
        if (strpos ($partialExcerpt, "<b>") === false) continue;
        // Store this bit of the excerpt.
        $excerpt .= "<p style=\"margin-top: 0em\">$partialExcerpt</p>\n";
      }

      // Display the search result.
      echo "<p style=\"margin-top: 0.75em; margin-bottom: 0em\"><a href=\"$url\">$title</a></p>\n";
      echo "$excerpt";
    }
  }
}


echo "<font size=\"-1\" color=\"grey\"><hr /></font>\n";
echo "</body>\n";


?>
