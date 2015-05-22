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
* Dialog that presents the user with a multi-line text area for text entry.
*/
class Dialog_Text
{
  private $view;

  /**
  * Text dialog constructor.
  * $header: What header to show.
  * $text: Initial text for the user to edit.
  * $parameter: The GET parameter to be used to save the text.
  */
  public function __construct ($header, $text, $parameter)
  {
    $this->view = new Assets_View (__FILE__);
    $this->view->view->caller = $_SERVER["PHP_SELF"];
    $this->view->view->header = $header;
    $this->view->view->parameter = $parameter;
    $this->view->view->text = $text;
  }


  public function info_top ($text)
  {
    $this->view->view->info_top = $text;
  }


  public function info_bottom ($text)
  {
    $this->view->view->info_bottom = $text;
  }


  public function run ()
  {
    $this->view->render ("text.php");
    Assets_Page::footer ();
    die ();
  }


}
?>
