<?php

require("constants.php");

// GET the channel to listen on.
$channel = stripslashes($_GET["channel"]);

while(1) {
  $handler = opendir($messagesdir);
  $i = 0;
  while ($file = readdir($handler)) {
    if ($file != '.' && $file != '..') {
      $parts=explode(".",$file);
      $suffix = $parts[count($parts)-1];
      if ("$suffix" == "$channel") {
        $array[$i] = $file;
        $i++;
      }
    }
  }
  closedir($handler);

  // Sort the file, and suppress errors in case there are no files to sort.
  @sort ($array);

  for ($i = 0; $i < sizeof ($array); $i++) {
    $filename = "$messagesdir/$array[$i]";
    $fh = fopen("$filename", 'r');
    $data = fread($fh, filesize ($filename));
    fclose($fh);
    echo "$data";
    unlink ("$filename");
    exit;
  }

  sleep(1);
}
?>
