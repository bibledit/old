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


require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);


$config_general = Database_Config_General::getInstance ();


// The query: The word or string to search for.
$queryString = isset($_GET['q'])?$_GET['q']:'';


$smarty = new Smarty_Bibledit (__FILE__);


// Put the query string into the search box.
Assets_Page::header (gettext ("Search"), $queryString);


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
$sphinxClient->SetServer ("localhost", (int) $config_general->getSearchDaemonPort ());
$sphinxClient->SetConnectTimeout (1);
$sphinxClient->SetArrayResult (true);
$sphinxClient->SetMatchMode (SPH_MATCH_EXTENDED2);
//include ("session/levels.php");
//$sphinxClient->SetFilter ("level", array (CONSULTANT_LEVEL, TRANSLATOR_LEVEL));
$sphinxClient->SetLimits (0, 10000);


$queryResult = $sphinxClient->Query ($queryString, "sphinxsearch");
if ($queryResult === false) {
  echo "<p>Search failed: " . $sphinxClient->GetLastError () . "</p>\n";
} else {
  

  if ($sphinxClient->GetLastWarning()) {
    echo "<p>Warning: " . $sphinxClient->GetLastWarning () . "</p>\n";
  }


  // Number of search results.
  $totalFound = $queryResult['total_found'];
  if ($queryString == "") $totalFound = 0;
  $smarty->assign ("totalFound", $totalFound);


  // Assemble the search results.
  $titles = array ();
  $urls = array ();
  $excerpts = array ();
  if ($totalFound > 0) {
    foreach ($queryResult["matches"] as $docinfo) {

      // The title.
      $title = $docinfo['attrs']['title'];
      $title = Filter_Html::sanitize ($title);
      $titles [] = $title;

      // The url.
      $url = $docinfo['attrs']['url'];
      $urls [] = $url;

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
      $excerpts [] = $excerpt;
    }
  }
  // Display the search results.
  $smarty->assign ("urls", $urls);
  $smarty->assign ("titles", $titles);
  $smarty->assign ("excerpts", $excerpts);
}


$smarty->display ("search.tpl");


Assets_Page::footer ();


?>
