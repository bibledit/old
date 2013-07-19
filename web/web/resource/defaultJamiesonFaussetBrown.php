<?php

// This sample script displays the text of
// Commentary Critical and Explanatory on the Whole Bible
// A commentary prepared by Robert Jamieson, A. R. Fausset and David Brown.
// Published in 1871.

// Bibledit-Web calls the script with three parameters on the command line.
$book = $argv [1];
$chapter = $argv [2];
$verse = $argv [3];

// On StudyLight.org, Genesis equals book 0.
$book--;

$url = "http://www.studylight.org/com/jfb/view.cgi?bk=$book&ch=$chapter";

$text = file_get_contents ($url);

// The following filter tries to locate the verse text.
$startTag = "\"" . $verse . "\"";
$rawLines = explode ("\n", $text);
$output = array ();
$withinVerse = false;
foreach ($rawLines as $line) {
  if (strpos ($line, "<hr") !== false) {
    $withinVerse = false;
  }
  if (strpos ($line, $startTag) !== false) {
    $withinVerse = true;
  }
  if ($withinVerse) {
    $output [] = $line;
  }
}

$text = implode ("\n", $output);

if ($text == "") {
  $text = "<a href=\"$url\" target=\"_blank\">$url</a>";
}

echo $text;

?>
