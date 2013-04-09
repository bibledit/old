<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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

class Assets_Page 
{
  public static function header ($title, $query = "")
  {
    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->assign ("title", $title);
    $smarty->assign ("query", $query);
    $smarty->display ("xhtml_start.tpl");
    $smarty->display ("header_full.tpl");
  }
  public static function success ($message)
  {
    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->assign ("message", $message);
    $smarty->display ("success.tpl");
  }
  public static function error ($message)
  {
    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->assign ("message", $message);
    $smarty->display ("error.tpl");
  }
  public static function message ($message)
  {
    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->assign ("message", $message);
    $smarty->display ("message.tpl");
  }
  public static function footer ()
  {
    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->display ("footer_full.tpl");
    $smarty->display ("xhtml_finish.tpl");
  }
}


?>
