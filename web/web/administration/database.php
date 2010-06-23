<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
include_once ("messages/messages.php");


$database_instance = Database_Instance::getInstance();
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("database.tpl");

// Show number of tables.
$result = $database_instance->runQuery ("SHOW TABLES;");
$smarty->assign ("tables_count_before", $result->num_rows);
message_information ("Number of tables in the database: " . $result->num_rows);
message_information ("Optimizing tables");
flush ();

// The logs table.
$database_logs = Database_Logs::getInstance ();
$database_logs->optimize();


// The versions table.
$database_versions = Database_Versions::getInstance ();
$database_versions->optimize ();


// The user table.
$database_users = Database_Users::getInstance();
$database_users->optimize ();


// The general configuration table.
$database_config_general = Database_Config_General::getInstance ();
$database_config_general->optimize ();


// The mail table.
$database_mail = Database_Mail::getInstance ();
$database_mail->optimize();


// The cron table.
$database_cron = Database_Cron::getInstance ();
$database_cron->optimize ();


// The mailer table.
$database_mailer = Database_Mailer::getInstance ();
$database_mailer->optimize ();


// The confirmations table.
$database_confirm = Database_Confirm::getInstance ();
$database_confirm->optimize ();


// The books table.
$database_books = Database_Books::getInstance ();
$database_books->optimize();


// The dialog table.
$database_dialog = Database_Dialog::getInstance ();
$database_dialog->optimize ();


// The versifications table.
$database_versifications = Database_Versifications::getInstance();
$database_versifications->optimize();


// The styles table.
$database_styles = Database_Styles::getInstance();
$database_styles->optimize();


// The user settings table.
$database_config_user = Database_Config_User::getInstance();
$database_config_user->optimize();


// The Bibles table.
$database_bibles = Database_Bibles::getInstance();
$database_bibles->optimize();


// The ipc table.
$database_ipc = Database_Ipc::getInstance();
$database_ipc->optimize();


// The Snapshots table.
$database_snapshots = Database_Snapshots::getInstance();
$database_snapshots->optimize();


// The repositories table.
$database_repositories = Database_Repositories::getInstance();
$database_repositories->optimize();


// The sessions table.
$database_sessions = Database_Sessions::getInstance();
$database_sessions->optimize();


// The consultation notes table.
$database_notes = Database_Notes::getInstance();
$database_notes->optimize();


// The shell table.
$database_shell = Database_Shell::getInstance();
$database_shell->optimize();


// Done.
message_information ("Ready");


?>
