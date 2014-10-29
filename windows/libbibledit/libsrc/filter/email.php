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


class Filter_Email
{

  /**
  * Extracts the pure email address from a string.
  * $input: Foo Bar <foo@bar.nl>
  * $input: foo@bar.nl
  * Returns: foo@bar.nl
  * If there is no valid email, it returns false.
  */
  public static function extractEmail ($input)
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
    $validator = new Zend_Validate_EmailAddress ();
    if (!$validator->isValid ($email)) return false;
    return $email;
  }


  /**
  * Extracts a clean string from the email body given in $input.
  * It leaves out the bit that was quoted.
  * If $year and $sender are given, it also removes lines that contain both strings.
  * This is used to remove lines like:
  * On Wed, 2011-03-02 at 08:26 +0100, Bibledit-Web wrote:
  */
  public static function extractBody ($input, $year = "", $sender = "")
  {
    $input = explode ("\n", $input);
    if ($input === false) return "";
    $body = array ();
    foreach ($input as $line) {
      $trimmed = trim ($line);
      if ($trimmed == "") continue;
      if (strpos ($trimmed, ">") === 0) continue;
      if (($year != "") && ($sender != "")) {
        if (strpos ($trimmed, $year) !== false) {
          if (strpos ($trimmed, $sender) !== false) {
            continue;
          }
        }
      }
      $body [] = $line;
    }
    $body = implode ("\n", $body);
    $body = trim ($body);
    return $body;
  }


  /**
  * Extracts the first text/plain message from a normal or a multipart email message.
  * $message: Zend_Mail message.
  * Returns: text/plain Zend_Mail message.
  */
  public static function extractPlainTextMessage ($message)
  {
    // If the message is not a MIME multipart message,
    // then the text/plain body part is the message itself.
    if (!$message->isMultipart ()) return $message;
    // This is a multipart message. Look for the plain text part.
    $foundPart = $message;
    foreach (new RecursiveIteratorIterator($message) as $part) {
      try {
        if (strtok($part->contentType, ';') == 'text/plain') {
          $foundPart = $part;
          break;
        }
      } catch (Zend_Mail_Exception $e) {
      }
    }
    return $foundPart;
  }


}

?>
