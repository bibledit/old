<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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

class Filter_Email
{

  /**
  * Extracts the pure email address from a string.
  * $input: Foo Bar <foo@bar.nl>
  * $input: foo@bar.nl
  * Returns: foo@bar.nl
  * If there is no valid email, it returns false.
  */
  public function extractEmail ($input)
  {
    $pos = strpos ($input, "<");
    if ($pos !== false) {
      $input = substr ($input, $pos + 1);
    }
    $pos = strpos ($input, ">");
    if ($pos !== false) {
       $input = substr ($input, 0, $pos);
    }
    $email = $input;
    unset ($input);
    if (!Validate_Email::valid ($email)) return false;
    return $email;
  }


  /**
  * Extracts a clean string from the email body.
  * It leaves out the bit that was quoted. 
  */
  public function extractBody ($input)
  {
    $input = explode ("\n", $input);
    if ($input === false) return "";
    $body = array ();
    foreach ($input as $line) {
      $trimmed = trim ($line);
      if ($trimmed == "") continue;
      if (strpos ($trimmed, "--=-") === 0) continue;
      if (strpos ($trimmed, "Content-") === 0) continue;
      if (strpos ($trimmed, "> ") === 0) break;
      $body [] = $line;
    }
    $body = implode ("\n", $body);
    return $body;
  }


}

?>

