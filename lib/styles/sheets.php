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


class Styles_Sheets
{


  // Recreates all stylesheet.css files through the CLI server.
  // The advantage of this is that it happens on the background,
  // so that the user interface becomes more responsive.
  public static function create_all ()
  {
    Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/csscli.php"));
  }


  // Recreates the various stylesheets.css files.
  public function recreate ()
  {
    $database_styles = Database_Styles::getInstance ();
    $stylesheets = $database_styles->getSheets ();
    foreach ($stylesheets as $stylesheet) {
      $path = $this->get_location ($stylesheet, false);
      $this->create ($stylesheet, $path, false, false);
      $path = $this->get_location ($stylesheet, true);
      $this->create ($stylesheet, $path, true, false);
    }
  }


  public function create ($stylesheet, $path, $editor, $export_bible)
  {
    $styles_css = new Styles_Css ($stylesheet);
    if ($editor) {
      $styles_css->editor ();
    }
    if ($export_bible) {
      $styles_css->exports ();
      $styles_css->customize ($export_bible);
    }
    $styles_css->generate ();
    $styles_css->css ($path);
  }


  public function get_location ($sheet, $editor)
  {
    $path = realpath ("..") . "/dyncss/";
    if ($editor) $path .= "editor";
    else $path .= "basic";
    $path .= "$sheet.css";
    return $path;
  }


}


?>
