<?php

  // Page bootstrapping.
  $GLOBALS["no_database_access_yet"] = true;
  require_once ("bootstrap/bootstrap.php");

  echo "<h2>Site setup</h2>";
  echo "<h3>Please scroll down to see all messages</h3>";
  flush ();

  // Info about the username of the web server.
  echo "<p>Ok: Web server username ";
  system ("whoami");
  echo "</p>";
  flush ();
   
  // Check that files are writeable.
  // Todo system ("chmod -R 0777 *");
  $files_writable = true;
  $handler = opendir(".");
  while ($file = readdir($handler)) {
    if ($file != '.' && $file != '..') {
      if (!is_writable ($file)) {
        $files_writable = false;
      }
    }
  }
  closedir($handler);
  if ($files_writable) {
    echo "<p>Ok: Files writeable</p>";
  } else {
    echo "<p>Error: Could not make files writeable</p>";
    die;
  }
  flush ();

  // Temporary directory writeable.
  if (is_writable (sys_get_temp_dir())) {
    echo "<p>Ok: Temporary directory writeable</p>";
  } else {
    echo "<p>Error: Temporary directory is not writeable</p>";
    die;
  }
  flush ();

  // If the user posted new values for accessing the database, write these to file.
  if (isset($_POST['submit'])) {
    $name   = $_POST['name'];
    $host   = $_POST['host'];
    $user   = $_POST['user'];
    $pass   = $_POST['pass'];
    $contents  = "<?php\n";
    $contents .= "\$database_host = \"$host\";\n";
    $contents .= "\$database_name = \"$name\";\n";
    $contents .= "\$database_user = \"$user\";\n";
    $contents .= "\$database_pass = \"$pass\";\n";
    $contents .= "?>\n";
    file_put_contents ("database/credentials.php", $contents);
  }

  // Try database access.
  $database_instance = Database_Instance::getInstance(true);
  if (!$database_instance->is_connected) {
    include ("database/credentials.php");
    echo "<p>Could not connect to database name $database_name running on host $database_host with username $database_user and password $database_pass.</p>";
    echo "<p>Please enter new values below, and press Submit to retry.</p>";
    echo "<form action=\"setup.php\" method=\"post\">";
    echo "<table>";
    echo "<tr>";
    echo "<td align=\"right\">Database name</td>";
    echo "<td><input type=\"text\" name=\"name\" value=\"$database_name\" /></td>";
    echo "</tr>";
    echo "<tr>";
    echo "<td align=\"right\">Database host</td>";
    echo "<td><input type=\"text\" name=\"host\" value=\"$database_host\" /></td>";
    echo "</tr>";
    echo "<tr>";
    echo "<td align=\"right\">Database username</td>";
    echo "<td><input type=\"text\" name=\"user\" value=\"$database_user\" /></td>";
    echo "</tr>";
    echo "<tr>";
    echo "<td align=\"right\">Database password</td>";
    echo "<td><input type=\"text\" name=\"pass\" value=\"$database_pass\" /></td>";
    echo "</tr>";
    echo "<tr>";
    echo "<td></td>";
    echo "<td><input type=\"submit\" name=\"submit\" value=\"Submit\" /></td>";
    echo "</tr>";
    echo "</table>";
    echo "</form>";
    die;
  }
  echo "<p>Ok: Database access</p>";
  flush ();

  // Creating tables.
  $result = $database_instance->runQuery ("SHOW TABLES;");
  echo "<p>Ok: Number of tables in the database before table maintenance: " . $result->num_rows . "</p>";
  flush ();
  
  // The logs table.
  $database_logs = Database_Logs::getInstance ();
  $database_logs->verify();
  unset ($database_logs);
  flush ();
  
  // The versions table.
  $database_versions = Database_Versions::getInstance ();
  $database_versions->verify ();
  unset ($database_versions);
  flush ();
  
  // The user table.
  $database_users = Database_Users::getInstance();
  $database_users->verify ();
  unset ($database_users);
  flush ();
  
  // The general configuration table.
  $database_config_general = Database_Config_General::getInstance ();
  $database_config_general->verify (); 
  unset ($database_config_general);
  flush ();
  
  // The mail table.
  $database_mail = Database_Mail::getInstance ();
  $database_mail->verify();
  unset ($database_mail);
  flush ();
  
  // The cron table.
  $database_cron = Database_Cron::getInstance ();
  $database_cron->verify ();
  unset ($database_cron);
  flush ();
  
  // The mailer table.
  $database_mailer = Database_Mailer::getInstance ();
  $database_mailer->verify ();
  unset ($database_mailer);
  flush ();
  
  // The confirmations table.
  $database_confirm = Database_Confirm::getInstance ();
  $database_confirm->verify ();
  unset ($database_confirm);
  flush ();
  
  // The books table.
  $database_books = Database_Books::getInstance ();
  $database_books->verify();
  $database_books->import();
  unset ($database_books);
  flush ();
  
  // The dialog table.
  $database_dialog = Database_Dialog::getInstance ();
  $database_dialog->verify ();
  unset ($database_dialog);
  flush ();
  
  // The versifications table.
  $database_versifications = Database_Versifications::getInstance();
  $database_versifications->verify();
  $database_versifications->import();
  unset ($database_versifications);
  flush ();

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

  // Show number of tables again.
  $result = $database_instance->runQuery ("SHOW TABLES;");
  echo "<p>Ok: Number of tables in the database after maintenance: " . $result->num_rows . "</p>";

  // Run the setup files for the plugins.
  // The exception handler prevents a buggy plugin from crippling the whole system.
  $plugin_enumerator = Plugins_Enumerator::getInstance();
  $plugins = $plugin_enumerator->getSetupPlugins ();
  foreach ($plugins as $plugin) {
    try {
      include ($plugin);
      $file = dirname (__FILE__) . "/" . $plugin;
      unlink ($file);
      echo "<p>Ok: Setup has erased the plugin setup file $plugin</p>";
    } catch (Exception $e) {
      $message = $e->getMessage ();
      $database_log->log ($message);
    }
  }

  // Script erases itself.
  unlink (__FILE__);
  echo "<p>Ok: Setup has erased itself</p>";

  echo "<p>Ok: Setup completed successfully</p>";
?>
