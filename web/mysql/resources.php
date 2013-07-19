<?php

// The purpose of this file is to create a file with SQL statements.
// The SQL statements contain the data for the default external resources.

$folder = $argv [1];
$files = array ();
foreach (new DirectoryIterator ($folder) as $fileInfo) {
  if($fileInfo->isDot()) continue;
  if($fileInfo->isDir()) continue;
  $files [] = $fileInfo->getFilename();
}

echo "USE `BIBLEDITDATABASE`; \n";

foreach ($files as $file) {
 
  if (strpos ($file, "default") === false) continue;
  $suffix = pathinfo ($file, PATHINFO_EXTENSION);
  $name = substr ($file, 7, -1 - strlen ($suffix));
  $name = "$name (bibledit)";
  $code = file_get_contents ("$folder/$file");

  echo "DELETE FROM resources WHERE name = '$name'; \n";
  echo "INSERT INTO resources VALUES ( \n";
  echo "NULL, \n";
  echo "'$name', \n";
  echo "'php script', \n";
  echo "'$code'); \n";
  
}

?>
