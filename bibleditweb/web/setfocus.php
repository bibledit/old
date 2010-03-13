<?php

require_once ("bootstrap/bootstrap.php");

$book = $_GET['book'];
$chapter = $_GET['chapter'];
$verse = $_GET['verse'];
$versification = $_GET['versification'];

$database_focus = Database_Focus::getInstance();
$database_focus->storeFocus ($book, $chapter, $verse);


?>
