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


class Dialog_Yes2
{
  private $view;

  /**
  * Dialog that asks the user for confirmation to perform an action.
  * $question: The question to ask.
  * $action: String with the query parameter to add to the basic query.
  * In case the user gives confirmation, then the following parameters are added: &confirm=yes
  */
  public function __construct ($question, $action)
  {
    $this->view = new Assets_View (__FILE__);
    $caller_url = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ());
    $this->view->view->caller_url = $caller_url;
    $this->view->view->action = $action;
    $this->view->view->question = $question;
    $this->view->render ("yes2.php");
    Assets_Page::footer ();
    die ();
  }
}


?>
