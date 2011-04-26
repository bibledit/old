<?php
// cd into the bibledit/web/web/locale directory.
// Run this script: php test.php
putenv("LANGUAGE=nl");
setlocale(LC_MESSAGES,"");
bindtextdomain("bibledit", "."); 
textdomain("bibledit");
echo gettext ("Bijbel");
echo "\n";
?>
