<?php

// This sample script displays the text of
// Commentary Critical and Explanatory on the Whole Bible
// A commentary prepared by Robert Jamieson, A. R. Fausset and David Brown.
// Published in 1871.

// Bibledit-Web calls the script with three parameters on the command line.
$book = $argv [1];
$chapter = $argv [2];
$verse = $argv [3];

// The directory on studylight.org
$directory = "jfb";

include ("studylight.php");

?>
