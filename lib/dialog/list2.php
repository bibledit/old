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
* Dialog that presents the user with a list of options and asks the user to pick one.
*/
class Dialog_List2
{
  private $view;
  private $text_lines;
  private $get_parameters;


  public function __construct ($header)
  {
    $this->view = new Assets_View (__FILE__);
    $caller_url = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ());
    $this->view->view->caller_url = $caller_url;
    $this->view->view->header = $header;
    $this->view->view->info_top = Locale_Translate::_("Here are the various options:");
    $this->view->view->info_bottom = Locale_Translate::_("Please pick one.");
  }


  public function info_top ($text)
  {
    $this->view->view->info_top = $text;
  }


  public function info_bottom ($text)
  {
    $this->view->view->info_bottom = $text;
  }


  public function horizontal ()
  {
    $this->view->view->horizontal = true;
  }


  public function add_row ($text_line, $get_parameter)
  {
    $this->text_lines[]     = $text_line;
    $this->get_parameters[] = $get_parameter;
  }


  public function run ()
  {
    $this->view->view->text_lines = $this->text_lines;
    $this->view->view->get_parameters = $this->get_parameters;
    $this->view->render ("list2.php");
    Assets_Page::footer ();
    die ();
  }


}
?>
