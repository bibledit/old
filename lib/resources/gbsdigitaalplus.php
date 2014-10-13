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


// This script processes and displays Bible data from gbsdigitaal.nl
// plus extra content like headers, introductions, cross references, and notes.
// It is supposed to be included in a more specific script.

function gbsdigitaalplus ($url, $chapter, $verse)
{
  
  $json = file_get_contents ($url);
  $data = json_decode ($json);
  
  
  // print_r ($data); die;
  
  
  $output = "";
  
  
  if ($chapter == 0) {
    @$introduction = $data->summary;
    $introduction = str_replace (array ("\n", "\r"), " ", $introduction);
    $introduction = str_replace ("\t", "", $introduction);
    $introduction = str_replace ('  ', ' ', $introduction);
    $introduction = str_replace ('<br />', "", $introduction);
    $introduction = trim ($introduction);
    $output .= "<p>$introduction</p>";
  }
  
  
  if ($verse == 0) {
    @$introduction = $data->summary->originalText;
    $introduction = str_replace (array ("\n", "\r"), " ", $introduction);
    $introduction = str_replace ("\t", "", $introduction);
    $introduction = str_replace ('  ', ' ', $introduction);
    $introduction = trim ($introduction);
    $output .= "<p>$introduction</p>";
  }
  
  
  if (isset ($data->verses)) {
    $verses = $data->verses;
    foreach ($verses as $offset => $data) {
      if (isset ($data->number)) {
        // Verse match.
        if ($data->number == $verse) {
          // Add any header.
          if ($offset) {
            $object = $verses [$offset - 1];
            if (isset ($object->isHeader)) {
              $text = $object->text;
              $output .= "<p><i>$text</i></p>";
            }
          }
          // Add verse text.
          if (isset ($data->text)) {
            $text = $data->text;
            $text = str_replace ('<br />', "", $text);
            $output .= $text;
          }
          // Add notes.
          if (isset ($data->commentaries)) {
            foreach ($data->commentaries as $offset => $commentary) {
              $number = $commentary->number;
              $text = $commentary->originalText;
              $references = $commentary->references;
              foreach ($references as &$reference) {
                $reference = array ('passage' => $reference->id, 'start' => $reference->startIndex, 'length' => $reference->length, 'title' => $reference->text);
              }
              // Invert the references, so we start replacing from the back of the string,
              // so the lengths and offsets are not affected.
              // Insert link, e.g.: <a class="navigate" href="19.90.2">Ps. 90:2</a>
              $references = array_reverse ($references);
              foreach ($references as $reference) {
                $passage = $reference ['passage'];
                $start = $reference ['start'];
                $length = $reference ['length'];
                $title = $reference ['title'];
                $title = preg_replace('/<sup[^<]+(<\/sup>)/i', '', $title);
                $title = strip_tags ($title);
                $text = mb_substr ($text, 0, $start) . '<a class="navigate" title="' . $title . '" href="' . $passage . '">'
                      . mb_substr ($text, $start, $length) . "</a>"
                      . mb_substr ($text, $start + $length);
              }
              $output .= "<p>$number $text</p>";
            }
          }
        }
      }
    }
  }
 
  
  $output .= "\n";
  return $output;
}


?>
