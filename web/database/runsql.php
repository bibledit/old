<?php

// Page bootstrapping.
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

$query = $_GET['sql']; // The MySQL query to run.
$goto = $_GET['goto']; // The URL to go to after done.

// It seems that a GET request inserts backslashes before the single quotes.
// Therefore these should be removed.
$query = str_replace ("\\", "", $query);

// Execute the SQL query.
$database = Database_Instance::getInstance ();
$database->mysqli->query ($query);
header ("Location: $goto");

?>
