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

// This script filters commentaries from www.studylight.org.


function studylight ($directory, $book, $chapter, $verse)
{
  // On StudyLight.org, Genesis equals book 0, Exodus book 1, and so on.
  $book--;
  
  $url = "http://www.studylight.org/com/$directory/view.cgi?bk=$book&ch=$chapter";
  
  // Get the html from the server, and tidy it up.
  @$html = file_get_contents ($url);
  $tidy = Tidy_Html::run ($html);
  $tidied = explode ("\n", $tidy);

  $relevant_lines = array ();
  $relevant_flag = false;
    
  foreach ($tidied as $line) {
    
    if ($relevant_flag) {
      $relevant_lines [] = $line;
    }

    $pos = strpos ($line, '</div>');
    if ($pos !== false) $relevant_flag = false;

    $pos = strpos ($line, 'name="' . $verse . '"');
    if ($pos !== false) $relevant_flag = true;
    
  }
  
  $html = implode ("\n", $relevant_lines);
  $html .= "<p><a href=\"$url\" target=\"_blank\">$url</a></p>\n";

  return $html;
}


?>
