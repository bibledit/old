<?php

// This sample script displays the text of
// Commentary Critical and Explanatory on the Whole Bible
// A commentary prepared by Robert Jamieson, A. R. Fausset and David Brown.
// Published in 1871.

// Bibledit-Web calls the script with three parameters on the command line.
$book = $argv [1];
$chapter = $argv [2];
$verse = $argv [3];

// On StudyLight.org, Genesis equals book 0, Exodus book 1, and so on.
$book--;

$url = "http://www.studylight.org/com/jfb/view.cgi?bk=$book&ch=$chapter";

$text = file_get_contents ($url);

$pos = strpos ($text, 'name="' . $verse . '"');
if ($pos === false) {
  $text = "";
} else {
  $text = substr ($text, $pos + 8 + strlen ($verse));
}
$pos = strpos ($text, "<hr");
if ($pos === false) {
  $text = "";
} else {
  $text = substr ($text, 0, $pos);
}


$text .= "<p><a href=\"$url\" target=\"_blank\">$url</a></p>\n";


echo $text;

?>
