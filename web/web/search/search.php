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
$sphinxClient->SetServer ("localhost", 9312);
$sphinxClient->SetConnectTimeout (1);
$sphinxClient->SetArrayResult (true);
$sphinxClient->SetMatchMode (SPH_MATCH_EXTENDED2);
// $sphinxClient->SetFilter ("manual", array (MANUALIDENTIFIER));
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
      $title = $docinfo['attrs']['title'];
      $url = $docinfo['attrs']['url'];
      $text = $docinfo['attrs']['text'];
      $excerpt = $text;
      
      
      $titles [] = $title;
      $urls [] = $url;
      $excerpts [] = $excerpt;
    }
/*
			$options = array
			(
				"before_match" => "<b>",
				"after_match"	=> "</b>",
				"chunk_separator"	=> " ... ",
				"limit"	=> 60,
				"around" => 3,
				"exact_phrase" => 0
			);
      $excerpt = $sphinxClient->BuildExcerpts (array ($documentText), "sphinxsearch", $queryString, $options);
      $excerpt = $excerpt [0];
      echo "<p style=\"margin-top: 0em\">$excerpt</p>\n";
 
  */
  }
  // Display the search results.
  $smarty->assign ("urls", $urls);
  $smarty->assign ("titles", $titles);
  $smarty->assign ("excerpts", $excerpts);
}


$smarty->display ("search.tpl");


Assets_Page::footer ();


?>
