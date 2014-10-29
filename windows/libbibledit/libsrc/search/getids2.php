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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


// Get search variables from the query.
@$bible = $_GET ['b'];
@$searchfor = $_GET ['q'];
@$casesensitive = ($_GET ['c'] == "true");
@$searchplain = ($_GET ['p'] == "true");


$database_search = Database_Search::getInstance ();


// Do the search.
if ($casesensitive) {
  if ($searchplain) {
    $hits = $database_search->searchBibleTextCaseSensitive ($bible, $searchfor);
  } else {
    $hits = $database_search->searchBibleUsfmCaseSensitive ($bible, $searchfor);
  }
} else {
  if ($searchplain) {
    $hits = $database_search->searchBibleText ($bible, $searchfor);
  } else {
    $hits = $database_search->searchBibleUsfm ($bible, $searchfor);
  }
}


// Output identifiers of the search results.
foreach ($hits as $hit) {
  echo "$hit\n";
}


?>
