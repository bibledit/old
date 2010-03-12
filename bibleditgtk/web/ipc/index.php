<html>

<head>
<title>Messages</title>
<meta http-equiv="refresh" content="1">
<link rel="stylesheet" type="text/css" href="../css/stylesheet.css" />
</head>

<body>

<h2>Messages in the interprocess communication queue</h2>

<?php

require("constants.php");
$handler = opendir($messagesdir);
$i = 0;
while ($file = readdir($handler)) {
  if ($file != '.' && $file != '..' && $file != 'identifier') {
    $array[$i] = $file;
    $i++;
  }
}
closedir($handler);

// Sort the file, and suppress errors in case there are no files to sort.
@sort ($array);

for ($i = 0; $i < sizeof ($array); $i++) {
  echo "Message $array[$i]<br>\n";
  $filename = "$messagesdir/$array[$i]";
  $fh = fopen("$filename", 'r');
  $data = fread($fh, filesize ($filename));
  fclose($fh);
  echo "$data<br>\n";
  echo "<br>\n";
}

?>

</body>

</html>



