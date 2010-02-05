<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);


$database_instance = Database_Instance::getInstance(true);
$smarty = new Smarty_Bibledit (__FILE__);



$result = $database_instance->mysqli->query ("show tables;");
$smarty->assign ("tables_count_before", $result->num_rows);

// Create the table structures if these do not yet exist.
    
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


$result = $database_instance->mysqli->query ("show tables;");
$smarty->assign ("tables_count_after", $result->num_rows);
$smarty->display("database.tpl");


?>
