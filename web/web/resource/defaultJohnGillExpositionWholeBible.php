<?php

// This sample script displays the text of
// Matthew Poole, English Annotations on the Holy Bible.

// Bibledit-Web calls the script with three parameters on the command line.
$book = $argv [1];
$chapter = $argv [2];
$verse = $argv [3];

// The directory on studylight.org
$directory = "geb";

include ("studylight.php");

?>
