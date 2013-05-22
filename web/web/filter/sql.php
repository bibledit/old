<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


class Filter_Sql
{


  public static function notesSelectIdentifier ()
  {
    return " SELECT identifier ";
  }

  
  public static function notesConsiderPrivacy ($userlevel)
  {
    return " AND (private <= $userlevel) ";
  }


  public static function notesOptionalFulltextSearchRelevanceStatement ($search)
  {
    if ($search == "") return;

    $search = str_replace (",", "", $search);
    $reversedsearch = Filter_String::reverse ($search);
    $search = Database_SQLInjection::no ($search);
    $reversedsearch = Database_SQLInjection::no ($reversedsearch);

    $query = " , ";
    // Matches in natural language mode have much higher relevance.
    $query .= " (MATCH (cleantext) AGAINST ('$search' IN NATURAL LANGUAGE MODE) * 10) ";
    $query .= " + ";
    // Matches in phrase search have higher relevance.
    $query .= " (MATCH (cleantext) AGAINST ('\"$search\"' IN BOOLEAN MODE) * 5) ";
    $query .= " + ";
    // Prefix and suffix wildcards to the search words and search in boolean mode.
    $query .= " (MATCH (cleantext) AGAINST ('$search*' IN BOOLEAN MODE)) + (MATCH (reversedtext) AGAINST ('$reversedsearch*' IN BOOLEAN MODE) * 1) ";
    // Create relevance column.
    $query .= " AS relevance ";

    return $query;
  }


  public static function notesFromWhereStatement ()
  {
    return " FROM notes WHERE TRUE ";
  }  
  

  public static function notesOptionalFulltextSearchStatement ($search)
  {
    if ($search == "") return;
    $search = str_replace (",", "", $search);
    $reversedsearch = Filter_String::reverse ($search);
    $search = Database_SQLInjection::no ($search);
    $reversedsearch = Database_SQLInjection::no ($reversedsearch);
    $query = " AND (MATCH (cleantext) AGAINST ('$search*' IN BOOLEAN MODE)) OR (MATCH (reversedtext) AGAINST ('$reversedsearch*' IN BOOLEAN MODE)) ";
    return $query;
  }


  public static function notesOrderByRelevanceStatement ()
  {
    return " ORDER BY relevance DESC ";
  }  


}

?>
