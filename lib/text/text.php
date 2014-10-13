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


/**
* Class for creating Text documents.
*/
class Text_Text
{

  private $output;
  private $line;


  public function __construct ()
  {
    $this->output = array ();
    $this->line = "";
  }


  public function paragraph ($text = "")
  {
    if ($this->line != "") {
      // The filter that converts from USFM to clear texts inserts some stuff
      // that's being removed here again, as not desirable in clear text.
      $this->line = str_replace (Filter_Character::enSpace(), " ", $this->line);
      $this->line = str_replace ("  ", " ", $this->line);
      $this->output [] = $this->line;
      $this->line = "";
    }
    $this->text ($text);
  }


  public function line ()
  {
    return $this->line;
  }


  public function text ($text)
  {
    $this->line .= $text;
  }


  public function get ()
  {
    $this->paragraph ();
    $text = implode ("\n", $this->output);
    return $text;
  }


  public function save ($name)
  {
    $text = $this->get ();
    file_put_contents ($name, $text);
  }


}


?>
