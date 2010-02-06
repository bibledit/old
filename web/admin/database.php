<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);


$database_instance = Database_Instance::getInstance(true);
$smarty = new Smarty_Bibledit (__FILE__);



// Show number of tables.
$result = $database_instance->mysqli->query ("SHOW TABLES;");
$smarty->assign ("tables_count_before", $result->num_rows);


// The user table.
$str = <<<EOD
CREATE TABLE IF NOT EXISTS users (
username varchar(30) primary key,
password varchar(32),
id varchar(32),
level tinyint(1) unsigned not null,
email varchar(256),
timestamp int(11) unsigned not null
);
EOD;
$database_instance->mysqli->query ($str);
$database_instance->mysqli->query ("OPTIMIZE TABLE users;");


// The general configuration table.
$str = <<<EOD
CREATE TABLE IF NOT EXISTS config_general (
ident VARCHAR(100) NOT NULL,
value VARCHAR(1000),
offset INT NOT NULL
);
EOD;
$database_instance->mysqli->query ($str);
$database_instance->mysqli->query ("OPTIMIZE TABLE config_general;");


// Show number of tables again.
$result = $database_instance->mysqli->query ("SHOW TABLES;");
$smarty->assign ("tables_count_after", $result->num_rows);


// Display page.
$smarty->display("database.tpl");


?>
