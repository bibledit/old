<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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

class Filter_Html
{

  public static function sanitize ($html)
  {
    $html = htmlspecialchars ($html, ENT_QUOTES, "UTF-8");
    return $html;
  }


  public static function html2text ($html)
  {
    $html = html_entity_decode ($html);
    $html = str_replace ("\n", "", $html);
    $html = str_replace ("<p>", "\n", $html);
    $html = str_replace ("</p>", "", $html);
    $html = str_replace ("<div>", "\n", $html);
    $html = str_replace ("</div>", "", $html);
    $html = trim ($html);
    return $html;
  }

}

?>
