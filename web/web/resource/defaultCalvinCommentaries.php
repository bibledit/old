<?php

// This sample script displays the text of the Commentaries of Calvin.

// Bibledit-Web calls the script with three parameters on the command line.
$book = $argv [1];
$chapter = $argv [2];
$verse = $argv [3];

// The directory on studylight.org
$directory = "cal";

include ("studylight.php");

?>
