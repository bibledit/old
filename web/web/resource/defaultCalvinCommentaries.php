<?php

// This sample script displays the text of the Commentaries of Calvin.

// Bibledit-Web calls the script with three parameters on the command line.
$book = $argv [1];
$chapter = $argv [2];
$verse = $argv [3];

// On StudyLight.org, Genesis equals book 0.
$book--;

$url = "http://www.studylight.org/com/cal/view.cgi?bk=$book&ch=$chapter";

$text = file_get_contents ($url);

// The following filter tries to find the text that belongs to the verse,
// leaving out the rest.
$rawLines = explode ("\n", $text);
$output = array ();
$withinVerse = false;
foreach ($rawLines as $line) {
  if (strpos ($line, "<hr") !== false) {
    $withinVerse = false;
  }
  if ($withinVerse) {
    $output [] = $line;
  }
  if (strpos ($line, ">Verse $verse<") !== false) {
    $withinVerse = true;
  }
}

$text = implode ("\n", $output);

echo $text;

echo "\n";

?>
