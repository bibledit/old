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


class Dialog_List
{

  private $view;
  private $text_lines = array ();
  private $get_parameters;


  /**
  * Dialog that presents the user with a list of options and asks the user to pick one.
  * $query: Array with the basic query parameters for the page where to go on clicking Cancel or making a choice.
  * $info_top, $info_bottom - If these are left empty, they take standard values.
  * $horizontal - if true the list of options shows horizontally, rather than vertically, and the $info_ does not show.
  */
  public function __construct ($query, $header, $info_top, $info_bottom, $horizontal = false)
  {
    $this->view = new Assets_View (__FILE__);

    $caller_url = $_SERVER["PHP_SELF"];

    if (is_array ($query)) {
      $full_query = array ();
      foreach ($query as $value) {
        $full_query = array_merge ($full_query, array ($value => $_GET[$value]));
      }
      $caller_url .= "?" . http_build_query ($full_query);
    }

    $this->view->view->caller_url = $caller_url;

    $this->view->view->header = $header;

    if ($info_top == "")
      $info_top = Locale_Translate::_("Here are the various options:");
    $this->view->view->info_top = $info_top;

    if ($info_bottom == "")
      $info_bottom = Locale_Translate::_("Please pick one.");
    $this->view->view->info_bottom = $info_bottom;

    $this->view->view->horizontal = $horizontal;
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
    $this->view->render ("list.php");
    Assets_Page::footer ();
  }


}
?>
