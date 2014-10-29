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


class Filter_Html
{

  public static function sanitize ($html)
  {
    @$html = htmlspecialchars ($html, ENT_QUOTES, "UTF-8");
    return $html;
  }


  public static function html2text ($html)
  {
    // Clean the html up.
    $html = html_entity_decode ($html);
    $html = str_replace ("\n", "", $html);

    // The output text.
    $text = "";

    // Keep going while the html contains the < character.
    $pos = strpos ($html, '<');
    while ($pos !== false) {
      // Add the text before the <.
      $text .= substr ($html, 0, $pos);
      $html = substr ($html, $pos + 1);
      // Certain tags start new lines.
      $tag1 = strtolower (substr ($html, 0, 1));
      $tag2 = strtolower (substr ($html, 0, 2));
      $tag3 = strtolower (substr ($html, 0, 3));
      if  (($tag1 == "p")
        || ($tag3 == "div")
        || ($tag2 == "li")
        || ($tag3 == "/ol")
        || ($tag3 == "/ul")
        || ($tag2 == "h1")
        || ($tag2 == "h2")
        || ($tag2 == "h3")
        || ($tag2 == "h4")
        || ($tag2 == "h5")
        || ($tag2 == "br")
         ) {
        $text .= "\n";
      }
      // Clear text out till the > character.
      $pos = strpos ($html, '>');
      if ($pos !== false) {
        $html = substr ($html, $pos + 1);
      }
      // Next iteration.
      $pos = strpos ($html, '<');
    }
    // Add any remaining bit of text.
    $text .= $html;

    while (strpos ($text, "\n\n") !== false) {
      $text = str_replace ("\n\n", "\n", $text);
    }
    $text = trim ($text);
    return $text;
  }

}

?>
