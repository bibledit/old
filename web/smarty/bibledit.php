<?php

@include_once ("../bootstrap/bootstrap.php");
require_once ("bootstrap/bootstrap.php");
require_once ("smarty/Smarty.class.php");


class Smarty_Bibledit extends Smarty { 

  public function __construct($calling_file_php)
  {
    // Construct the Smarty object.
    parent::__construct();

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

    $session_logic = Session_Logic::getInstance ();
    if ($session_logic->loggedIn ()) {
      $this->assign ("user", $session_logic->currentUser()); 
    }
    
  } 
}
      
?>
