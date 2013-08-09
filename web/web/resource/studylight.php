<?php

// This script filters commentaries from www.studylight.org.

// On StudyLight.org, Genesis equals book 0, Exodus book 1, and so on.
$book--;

$url = "http://www.studylight.org/com/$directory/view.cgi?bk=$book&ch=$chapter";

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
