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

class Assets_Header
{
  private static $instance;
  private $view;
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
  * Writes a header with jQuery basics if needed.
  */
  public function jQueryHeader ($title)
  {
    $this->view = new Assets_View (__FILE__);
    $this->view->view->title = $title;
    
  }

  /**
  * Adds a document ready function for jQuery.
  */
  public function jQueryHeaderAddDocumentReadyFunction ($code)
  {
    if ($code != "") {
      $this->document_ready_functions[] = $code;
    }
  }

  /**
  * Adds a document ready function for jQuery.
  */
  public function jQueryHeaderAddWysiwygHeaders ()
  {
    $this->view->view->wysiwyg_editor = true;
    if (isset ($code)) {
      $this->document_ready_functions[] = $code;
    }
  }

  /**
  * Runs the header.
  */
  public function run ()
  {
    $this->view->view->document_ready_functions =  $this->document_ready_functions;
    $this->view->render ("xhtml_start.php");
    $this->view->render ("header_full.php");
    
  }

}


?>
