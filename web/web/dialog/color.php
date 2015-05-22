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


class Dialog_Color
{
  private $view;

  public function __construct ($query, $question, $item)
  {
    $this->view = new Assets_View (__FILE__);
    $base_url = $_SERVER['PHP_SELF'];
    if (is_array ($query)) {
      $base_url .= "?" . http_build_query ($query);
      $active_url = $base_url . "&";
    } else {
      $active_url = $base_url . "?";
    }
    $active_url .= $item . "=";
    $this->view->view->base_url = $base_url;
    $this->view->view->active_url = $active_url;
    $this->view->view->question = $question;
    $this->view->render ("color.php");
    Assets_Page::footer ();
  }
}


?>
