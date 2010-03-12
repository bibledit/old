<?php
/**
* @package smarty
*/


/**
* Needs to include the parent Smarty class.
*/
require_once ("smarty/Smarty.class.php");




/**
* Specialized form of Smarty, suited to our needs
*
* @package smarty
*/
class Smarty_Bibledit extends Smarty { 

  public function __construct($calling_file_php)
  {
    // Construct the Smarty object.
    parent::__construct();

    // No caching at all. It has been seen to confuse Smarty at times.
    $this->caching = 0;
    $this->compile_check = true;
    $this->force_compile = true;
    $this->clear_all_cache ();
    
    // Register the {t}Text{/t} gettext blocks.
    require_once('smarty-gettext/smarty-gettext.php');
    $this->register_block('t', 'smarty_translate');

    // Four important directories for Smarty.
    $this->template_dir = dirname($calling_file_php);
    $this->compile_dir = dirname(__FILE__) . '/templates_c';
    $this->config_dir = dirname(__FILE__) . '/configs';
    $this->cache_dir = dirname(__FILE__) . '/cache';

    // Modifier for the links in the headers.
    $bibledit_root_folder = Bootstrap::getInstance ()->bibledit_root_folder;    
    $file_path = dirname ($calling_file_php);
    $header_path_modifier = "";
    $iterations_count = 0;
    while (($file_path != $bibledit_root_folder) && (iterations_count < 10)) {
      $file_path = dirname ($file_path);
      $iterations_count++;
      $header_path_modifier .= "../";
    } 
    $this->assign("header_path_modifier", $header_path_modifier);

   
  }
 
  /**
  * This function display first sets session variables, then calls the parent display function.
  * Setting the session variables in Smarty is postponed to the very last moment, since 
  * these could change within the course of the calling .php file.
  */ 
  public function display($resource_name, $cache_id = null, $compile_id = null)
  {
    $session_logic = Session_Logic::getInstance ();
    if ($session_logic->loggedIn ()) {
      $this->assign ("user", $session_logic->currentUser()); 
      $this->assign ("level", $session_logic->currentLevel(true)); 
    }
    parent::display($resource_name, $cache_id, $compile_id);
  }
 
}
      
?>
