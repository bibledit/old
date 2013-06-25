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

$database = Database_Logs::getInstance ();
$database->optimize();

$database = Database_Users::getInstance();
$database->optimize ();

$database = Database_Config_General::getInstance ();
$database->optimize ();

$database = Database_Mail::getInstance ();
$database->optimize();

$database = Database_Mailer::getInstance ();
$database->optimize ();

$database = Database_Confirm::getInstance ();
$database->optimize ();

$database = Database_Books::getInstance ();
$database->optimize();

$database = Database_Dialog::getInstance ();
$database->optimize ();

$database = Database_Versifications::getInstance();
$database->optimize();

$database = Database_Styles::getInstance();
$database->optimize();

$database = Database_Config_User::getInstance();
$database->optimize();

$database = Database_Bibles::getInstance();
$database->optimize();

$database = Database_Ipc::getInstance();
$database->optimize();

$database = Database_Snapshots::getInstance();
$database->optimize();

$database = Database_Repositories::getInstance();
$database->optimize();

$database = Database_Sessions::getInstance();
$database->optimize();

$database = Database_Notes::getInstance();
$database->optimize();

$database = Database_Shell::getInstance();
$database->optimize();

$database = Database_Git::getInstance();
$database->optimize();

$database = Database_Logger::getInstance ();
$database->optimize();

$database = Database_Check::getInstance ();
$database->optimize();

$database = Database_Changes::getInstance ();
$database->optimize();

// Done.
message_information ("Ready");

Assets_Page::footer ();

?>
