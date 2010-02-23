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




Bootstrap::getInstance ();


require_once ("session/levels.php");


function page_access_level ($level) 
{
  $session_logic = Session_Logic::getInstance ();
  if ($level > $session_logic->currentLevel ()) {
    $smarty = new Smarty_Bibledit (__FILE__);
    $smarty->display("bootstrap.tpl");
    die ();
  } 
}



/**
* @package bibledit
*/
class Bootstrap
{
  private static $instance;
  public $bibledit_root_folder;

  
  private function __construct() {
    // Set the include path, where to look for included files.
    // This is important so as to make pointing to the included files much easier,
    // and to avoid tortuous path references.
    $this->bibledit_root_folder = dirname (dirname(__FILE__));
    $include_path = get_include_path () . ":" . $this->bibledit_root_folder;
    set_include_path ($include_path);
    ini_set('include_path', $include_path); 

    // Autoloader.
    // Automatically include the file that contains the $class_name.
    // E.g. class Database_Instance would require file database/instance.php.
    // Thus the name of the class determines which file gets required.
    // The above implies that all classes translate to files and folders in lower case.
    // An exception is made for the Zend_* classes.
    function __autoload($class_name)
    {
      if (substr ($class_name, 0, 4) != "Zend") {
        $class_name = strtolower ($class_name);
      }
      $path = str_replace("_", "/", $class_name);
      require_once $path.".php";
    }
    
    // No magic quotes: We prefer our own system.
    // No warnings to be given if this function no longer exists, such as in PHP 6.
    @set_magic_quotes_runtime(false);
  } 


  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Bootstrap();
    }
    return self::$instance;
  }
}




?>
