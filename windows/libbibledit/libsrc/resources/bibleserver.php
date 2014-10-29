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

// This script filters Bibles from www.bibleserver.com.

function bibleserver ($directory, $book, $chapter, $verse)
{

  // On bibleserver.com, Genesis is "1.Mose", Exodus is "2.Mose", and so on.
  
  $booknames = array (
    1 => "1.Mose",
    2 => "2.Mose",
    3 => "3.Mose",
    4 => "4.Mose",
    5 => "5.Mose",
    6 => "Josua",
    7 => "Richter",
    8 => "Rut",
    9 => "1.Samuel",
    10 => "2.Samuel",
    11 => "1.Könige",
    12 => "2.Könige",
    13 => "1.Chronik",
    14 => "2.Chronik",
    15 => "Esra",
    16 => "Nehemia",
    17 => "Esther",
    18 => "Hiob",
    19 => "Psalm",
    20 => "Sprüche",
    21 => "Prediger",
    22 => "Hoheslied",
    23 => "Jesaja",
    24 => "Jeremia",
    25 => "Klagelieder",
    26 => "Hesekiel",
    27 => "Daniel",
    28 => "Hosea",
    29 => "Joel",
    30 => "Amos",
    31 => "Obadja",
    32 => "Jona",
    33 => "Micha",
    34 => "Nahum",
    35 => "Habakuk",
    36 => "Zefanja",
    37 => "Haggai",
    38 => "Sacharja",
    39 => "Maleachi",
    40 => "Matthäus",
    41 => "Markus",
    42 => "Lukas",
    43 => "Johannes",
    44 => "Apostelgeschichte",
    45 => "Römer",
    46 => "1.Korinther",
    47 => "2.Korinther",
    48 => "Galater",
    49 => "Epheser",
    50 => "Philipper",
    51 => "Kolosser",
    52 => "1.Thessalonicher",
    53 => "2.Thessalonicher",
    54 => "1.Timotheus",
    55 => "2.Timotheus",
    56 => "Titus",
    57 => "Philemon",
    58 => "Hebräer",
    59 => "Jakobus",
    60 => "1.Petrus",
    61 => "2.Petrus",
    62 => "1.Johannes",
    63 => "2.Johannes",
    64 => "3.Johannes",
    65 => "Judas",
    66 => "Offenbarung"
  );
 
  
  $book = $booknames [$book];
  
  $url = "http://www.bibleserver.com/text/$directory/$book$chapter";
  
  $text = file_get_contents ($url);
  
  $pos = strpos ($text, 'no="' . $verse ."," . $verse . '"');
  if ($pos === false) {
    $text = "";
  } else {
    $text = substr ($text, $pos + 8 + strlen ($verse));
  }
  $pos = strpos ($text, "noscript");
  if ($pos === false) {
    $text = "";
  } else {
    $text = substr ($text, 0, $pos);
  }
  for ($i = 0; $i <= 9; $i++) {
    $text = str_replace ($i, "", $text);
  }
  
  $text = strip_tags ("<" . $text);
  $text = html_entity_decode ($text);
  $text = trim ($text);
  
  $text .= "<p><a href=\"$url\" target=\"_blank\">$url</a></p>\n";
  
  return $text;
}


?>
