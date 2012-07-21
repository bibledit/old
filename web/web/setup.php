<?php

  require_once ("bootstrap/bootstrap.php");

 
  
  $database_versifications = Database_Versifications::getInstance();
  $database_versifications->import();
die;

  // The styles table.
  $database_styles = Database_Styles::getInstance();
  $database_styles->verify();
  unset ($database_styles);
  flush ();
    
  // The user settings table.
  $database_config_user = Database_Config_User::getInstance();
  $database_config_user->verify();
  unset ($database_config_user);
  flush ();
    
  // The Bibles table.
  $database_bibles = Database_Bibles::getInstance();
  $database_bibles->verify();
  unset ($database_bibles);
  flush ();
    
  // The Snapshots table.
  $database_snapshots = Database_Snapshots::getInstance();
  $database_snapshots->verify();
  unset ($database_snapshots);
  flush ();
    
  // The ipc table.
  $database_ipc = Database_Ipc::getInstance();
  $database_ipc->verify();
  unset ($database_ipc);
  flush ();
    
  // The repositories table.
  $database_repositories = Database_Repositories::getInstance();
  $database_repositories->verify();
  unset ($database_repositories);
  flush ();
    
  // The sessions table.
  $database_sessions = Database_Sessions::getInstance();
  $database_sessions->verify();
  unset ($database_sessions);
  flush ();
    
  // The consultations notes table.
  $database_notes = Database_Notes::getInstance();
  $database_notes->verify();
  unset ($database_notes);
  flush ();

  // The shell table.
  $database_shell = Database_Shell::getInstance();
  $database_shell->verify();
  unset ($database_shell);
  flush ();

  // The git table.
  $database_git = Database_Git::getInstance();
  $database_git->verify();
  unset ($database_git);
  flush ();


?>
