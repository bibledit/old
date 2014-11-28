<?php
// Bump the version number in configure.ac.
// The line looks like this: 
// AC_INIT([bibledit], [1.0.1],[http://www.nongnu.org/bibledit])
$filename = __DIR__ . "/configure.ac";
$contents =filter_url_file_get_contents ($filename);
$pos = strpos ($contents, "AC_INIT");
$pos += 15;
$pos = strpos ($contents, "[", $pos);
$pos2 = strpos ($contents, "]", $pos);
$oldversion = substr ($contents, $pos + 1, $pos2 - $pos - 1);
$version = explode (".", $oldversion);
$version [2]++;
$newversion = implode (".", $version);
$contents = str_replace ("[$oldversion]", "[$newversion]", $contents);
file_put_contents ($filename, $contents);
?>
