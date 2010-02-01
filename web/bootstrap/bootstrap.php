<?php



Bootstrap::getInstance ();
Session_Logic::getInstance ();




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
    function __autoload($class_name)
    {
        $path = str_replace("_", "/", strtolower ($class_name));
        require_once $path.".php";
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
