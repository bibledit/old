<?php

require("constants.php");
while(1) {
  $handler = opendir($messagesdir);
  $i = 0;
  while ($file = readdir($handler)) {
    if ($file != '.' && $file != '..') {
      $parts=explode(".",$file);
      $suffix = $parts[count($parts)-1];
      if ("$suffix" == "bibletime") {
      $array[$i] = $file;
      $i++;
      }
    }
  }
  closedir($handler);

  // Sort the file, and suppress errors in case there are no files to sort.
  @sort ($array);

  for ($i = 0; $i < sizeof ($array); $i++) {

    $fh = fopen("$messagesdir/$array[$i]", 'r');
    $data = fread($fh, 1000);
    fclose($fh);
    echo "$data";
    unlink ("$messagesdir/$array[$i]");
    exit;
  }

  sleep(1);
}
?>
