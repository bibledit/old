<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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

class Assets_Header
{
  private static $instance;
  private $smarty;
  private $document_ready_functions;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Assets_Header();
    }
    return self::$instance;
  }
  /**
  * This works on a live object, and writes a header with jQuery basics if needed.
  */
  public function jQueryHeader ($title)
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $this->smarty->assign ("title", $title);
  }
  /**
  * This works on a live object, and adds a document ready function for jQuery.
  */
  public function jQueryHeaderAddDocumentReadyFunction ($code)
  {
    if (isset ($code)) {
      $this->document_ready_functions[] = $code;
    }
  }
  /**
  * This works on a live object, and adds a document ready function for jQuery.
  */
  public function run ()
  {
    $this->smarty->assign ("document_ready_functions", $this->document_ready_functions);
    $this->smarty->display ("xhtml_start.tpl");
    $this->smarty->display ("header_full.tpl");
  }
}


?>

