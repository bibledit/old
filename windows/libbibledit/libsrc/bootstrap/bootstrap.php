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


Bootstrap::getInstance ();


function page_access_level ($level)
{
  $session_logic = Session_Logic::getInstance ();
  if ($level > $session_logic->currentLevel ()) {
    $header = new Assets_Header ("Privileges");
    $header->setLogin ();
    $header->run ();
    $view = new Assets_View (__FILE__);
    $view->render ("privileges.php");
    Assets_Page::footer ();
    die ();
  }
}


class Bootstrap
{
  private static $instance;
  public $bibledit_root_folder;


  private function __construct()
  {

    // Default encoding.
    mb_internal_encoding ("UTF-8");
    
    // On shared hosting the temporal location may give read or write failures.
    // Set private temporal location for PHP.
    // Set private temporal location for SQLite.
    // http://stackoverflow.com/questions/10394517/setting-sqlite-temp-store-directory
    $tmpdir = realpath (__DIR__ . "/../tmp");
    putenv ("TMPDIR=$tmpdir");

    // Check whether to run the website setup script.
    // On Linux it is sufficient to check whether the "setup" folder exists.
    // But on Windows, this setup folder cannot be deleted, so it would exist always.
    // Therefore, to support Windows, it checks whether the index file in in the setup folder.
    if (file_exists ("../setup/index.php")) {
      $setupfolder = realpath ("../setup");
      $myfolder = realpath (".");
      if ($setupfolder != $myfolder) {
        include ("../filter/url.php");
        Filter_Url::redirect ("../setup/index.php");
        die;
      }
    }

    // Set the include path: Where to look for included files.
    $this->bibledit_root_folder = dirname (dirname(__FILE__));
    $include_path = get_include_path () . PATH_SEPARATOR . $this->bibledit_root_folder;
    set_include_path ($include_path);
    ini_set('include_path', $include_path);

    // Autoloader.
    // Automatically include the file that contains the $class_name.
    // E.g. class Database_Bibles would require file database/bibles.php.
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
    if ($timezone) date_default_timezone_set ($timezone);
    
    // Client mode setup.
    // In case the client mode is prepared, but not enabled,
    // the bootstrap script forwards to the client mode setup page,
    // unless it is already going to that page.
    if (Filter_Client::prepared ()) {
      if (!Filter_Client::enabled ()) {
        @$uri = $_SERVER ["REQUEST_URI"];
        $path = parse_url ($uri, PHP_URL_PATH);
        $folder = pathinfo ($path, PATHINFO_DIRNAME);
        $folder = basename ($folder);
        $page = pathinfo ($path, PATHINFO_BASENAME);
        if ($folder != "setup") {
          if ($page != "topbar.php") {
            if ($page != "client.php") {
              Filter_Url::redirect ("../administration/client.php");
              die;
            }
          }
        }
      }
    }
  }


  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Bootstrap();
    }
    return self::$instance;
  }
}


?>
