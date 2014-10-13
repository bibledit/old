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


class Dialog_Entry
{
  private $view;


  /**
  * Entry dialog constructor
  * $query   : Nothing, or array with query variables, e.g. array ("search" => "bibledit").
  *            If any $query is passed, if Cancel is clicked in this dialog, it should go go back
  *            to the original caller page  with the $query added. Same for Ok, it would post
  *            the value entered, but also add the $query to the url.
  * $question: The question to be asked.
  * $value   : The initial value to be put into the entry.
  * $submit  : Name of POST request to submit the information.
  * $help    : Help information explaining to the user what's going on.
  */
  public function __construct ($query, $question, $value, $submit, $help)
  {
    $this->view = new Assets_View (__FILE__);
    $base_url = $_SERVER['PHP_SELF'];
    if (is_array ($query)) {
      $base_url .= "?" . http_build_query ($query);
    }
    $this->view->view->base_url = $base_url;
    $this->view->view->question = $question;
    $this->view->view->value = $value;
    $this->view->view->submit = $submit;
    $this->view->view->help = $help;
    $this->view->render ("entry.php");
    Assets_Page::footer ();
  }
}


?>
