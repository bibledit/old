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


// This script processes and displays canonical data from gbsdigitaal.nl
// It is supposed to be included in a more specific script.


function gbsdigitaal ($url, $verse)
{
  $json = file_get_contents ($url);
  $data = json_decode ($json);
  
  
  $output = "";
  
  
  @$verses = $data->verses;
  if (is_array ($verses)) {
    foreach ($verses as $data) {
      if (isset ($data->number)) {
        if ($data->number == $verse) {
          if (isset ($data->text)) {
            $text = $data->text;
            $text = str_replace ('<br />', "", $text);
            $text = preg_replace('/<sup[^<]+(<\/sup>)/i', '', $text);
    
            // Remove the pilcrow sign / paragraph sign if it's there.
            $pos = mb_strpos ($text, "¶");
            if ($pos !== false) {
              $pos++;
              $text = mb_substr ($text, $pos);
            }
    
            $output = $text;
          }
        }
      }
    }
  }
  
  
  $output .= "\n";
  return $output;
}


?>
