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


class Assets_Page
{

  public static function header ($title, $searchQuery = "")
  {
    $header = new Assets_Header ($title);
    $header->setSearchQuery ($searchQuery);
    $header->run ();
  }

  public static function success ($message)
  {
    $view = new Assets_View (__FILE__);
    $view->view->message = $message;
    $view->render ("success.php");
  }

  public static function error ($message)
  {
    $view = new Assets_View (__FILE__);
    $view->view->message = $message;
    $view->render ("error.php");
  }

  public static function message ($message)
  {
    $view = new Assets_View (__FILE__);
    $view->view->message = $message;
    $view->render ("message.php");
  }

  public static function footer ()
  {
    $view = new Assets_View (__FILE__);
    $view->render ('footer.php');
    $view->render ('xhtml_finish.php');
  }

}


?>
