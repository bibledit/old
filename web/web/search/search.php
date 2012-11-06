<?php

/*

Copyright (©) 2012 Teus Benschop.

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
?>


<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  <meta content="text/html; charset=utf-8" http-equiv="content-type" />
  <title>Search</title>
  <link href="stylesheet.css" rel="stylesheet" type="text/css" />
</head>
<body>

<?php


require ("sphinxapi.php");


// The word to search for.
$queryString = isset($_GET['q'])?$_GET['q']:'';


// Display the header inclusing the search form
?>
<table class="navigation">
<tr>
<td><a href="http://www.stabiplan.com"><img src="stabiplanlogo.png"></a></td>
<td><a href="index.htm">_title_</a></td>
<td>
<form action="?" method="get" style="margin-left:12px;margin-top:6px;border-style: none;">
  <input type="text" name="q" id="q" size="40" value="<?php echo $queryString; ?>">	
  <input type="submit" value="Search">
</form>
</td>
<td><!--links--></td>
</tr>
</table>
<?php


$sphinxClient = new SphinxClient ();
$sphinxClient->SetServer ("localhost", 9312);
$sphinxClient->SetConnectTimeout (1);
$sphinxClient->SetArrayResult (true);
$sphinxClient->SetMatchMode (SPH_MATCH_EXTENDED2);
// The filter is used to search in one manual only.
$sphinxClient->SetFilter ("manual", array (MANUALIDENTIFIER));
$sphinxClient->SetLimits (0, 10000);


// Add wildcards if the query string is one word.
// If there is more than one word, just leave the query string as it is.
$updatedQueryString = trim ($queryString);
if (strpos ($updatedQueryString, " ") === false) $updatedQueryString = "*" . $updatedQueryString . "*";


$queryResult = $sphinxClient->Query ($updatedQueryString, "sphinxsearch");
if ($queryResult === false) {
  echo "<p>Query failed: " . $sphinxClient->GetLastError() . "</p>\n";
} else {
  

  if ($sphinxClient->GetLastWarning()) {
    echo "<p>Warning: " . $sphinxClient->GetLastWarning() . "</p>\n";
  }


  // Display the number of results found, and the query time.
  // It is put in a small grey font, so that it does not stand out.
  $totalFound = $queryResult['total_found'];
  echo "<p><font size=\"-1\" color=\"grey\">$totalFound results</font></p>\n";


  // Display the search results, and accompanying information.
  if ($totalFound > 0) {
    foreach ($queryResult["matches"] as $docinfo) {
			$documentID = $docinfo['id'];
			$documentWeight = $docinfo['weight'];
			$documentUrl = $docinfo['attrs']['url'];
			$documentTitle = $docinfo['attrs']['title'];
			$documentText = $docinfo['attrs']['text'];
      echo "<p style=\"margin-bottom: 0em\"><a href=\"$documentUrl\" title=\"$documentTitle\">$documentTitle</a></p>\n";
      $cleanUrl = str_replace('http://','', $documentUrl);
      echo "<p style=\"margin-top: 0em; margin-bottom: 0em\"><font size=\"-1\" color=\"green\">$cleanUrl</font></p>\n";
			$options = array
			(
				"before_match" => "<b>",
				"after_match"	=> "</b>",
				"chunk_separator"	=> " ... ",
				"limit"	=> 60,
				"around" => 3,
				"exact_phrase" => 0
			);
      $excerpt = $sphinxClient->BuildExcerpts (array ($documentText), "sphinxsearch", $updatedQueryString, $options);
      $excerpt = $excerpt [0];
      echo "<p style=\"margin-top: 0em\">$excerpt</p>\n";
    }
  }


  // Display the query statistics.
  // It is put in a small grey font, so that it does not stand out.
  // The query statistics is no longer displayed, because it displays the number of times a word was found in all manuals,
  // not just the current manual. This is confusing.
  /*
  if ($totalFound > 0) {
    echo "<p><font size=\"-1\" color=\"grey\">";
    foreach ($queryResult['words'] as $word => $info) {
      echo " Word '$word' was found in " . $info['docs'] . " documents.";
    }
    echo "</font></p>\n";
  }
  */
  // Display a small line to indicate the end of the list.
  echo "<font size=\"-1\" color=\"grey\">";
  echo "<hr />";
  echo "</font>\n";


}



?>

</body>
</html>
