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

    // Error handling, for debugging.
    // error_reporting(E_ALL);
    // ini_set('error_reporting', E_ALL);

    // Default encoding.
    mb_internal_encoding("UTF-8");

    // Set the include path, where to look for included files.
    $this->bibledit_root_folder = dirname (dirname(__FILE__));
    include $this->bibledit_root_folder . "/paths/paths.php";
    $include_path = get_include_path () . ":" . $this->bibledit_root_folder . ":" . dirname ($zendPath);
    set_include_path ($include_path);
    ini_set('include_path', $include_path); 
    
    // Autoloader.
    // Automatically include the file that contains the $class_name.
    // E.g. class Database_Instance would require file database/instance.php.
    // Thus the name of the class determines which file gets required.
    // The above implies that all classes translate to files and folders in lower case.
    // An exception is made for the Zend_* classes.
    function __autoload ($class_name)
    {
      if (substr ($class_name, 0, 4) != "Zend") {
        $class_name = strtolower ($class_name);
      }
      $path = str_replace ("_", "/", $class_name);
      require_once $path.".php";
    }
    // Register the function.
    spl_autoload_register ('__autoload');
    
    
    // Disable magic quotes.
    if ( (function_exists("get_magic_quotes_gpc")) && get_magic_quotes_gpc() ) {
      foreach($_GET    as $k => $v) $_GET   [$k] = stripslashes($v);
      foreach($_POST   as $k => $v) $_POST  [$k] = stripslashes($v);
      foreach($_COOKIE as $k => $v) $_COOKIE[$k] = stripslashes($v);
    }

    // General configuration database.    
    $database_config_general = Database_Config_General::getInstance();

    // The site's timezone.
    $timezone = $database_config_general->getTimezone ();
    date_default_timezone_set ($timezone);
    
    // The localization.
    $language = $database_config_general->getSiteLanguage ();
    if ($language != "") {
      putenv("LANGUAGE=$language");
      setlocale(LC_MESSAGES, "en_US.utf8"); // I am puzzled as to why this is needed to make gettext work.
      $textdomain = dirname (dirname (__FILE__)) . "/locale";
      bindtextdomain("bibledit", $textdomain); 
      textdomain("bibledit");
    }
      
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
