<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
include_once ("messages/messages.php");


$database_instance = Database_Instance::getInstance(true);
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("database.tpl");


// Show number of tables.
$result = $database_instance->mysqli->query ("SHOW TABLES;");
$smarty->assign ("tables_count_before", $result->num_rows);
message_information ("Number of tables in the database before the maintenance: " . $result->num_rows);
message_information ("Verifying lots of tables");

// The versions table.
$database_versions = Database_Versions::getInstance ();
$database_versions->verify ();


// The user table.
$database_users = Database_Users::getInstance();
$database_users->verify ();


// The general configuration table.
$database_config_general = Database_Config_General::getInstance ();
$database_config_general->verify ();


// The logs table.
$database_logs = Database_Logs::getInstance ();
$database_logs->verify();


// The mail table.
$database_mail = Database_Mail::getInstance ();
$database_mail->verify();


// The cron table.
$database_cron = Database_Cron::getInstance ();
$database_cron->verify ();


// The mailer table.
$database_mailer = Database_Mailer::getInstance ();
$database_mailer->verify ();


// The confirmations table.
$database_confirm = Database_Confirm::getInstance ();
$database_confirm->verify ();


// The books table.
$database_books = Database_Books::getInstance ();
$database_books->verify();
$database_books->import();


// The versifications table.
$database_versifications = Database_Versifications::getInstance();
$database_versifications->verify();
$database_versifications->import();


// Show number of tables again.
$result = $database_instance->mysqli->query ("SHOW TABLES;");
message_information ("Number of tables in the database after the maintenance: " . $result->num_rows);


?>
