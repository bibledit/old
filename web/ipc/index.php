<html>

<head>
<title>Messages</title>
<meta http-equiv="refresh" content="2">
</head>

<body>

<h2>Unread messages in the interprocess communication queue</h2>

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
  $fh = fopen("$messagesdir/$array[$i]", 'r');
  $data = fread($fh, 1000);
  fclose($fh);
  echo "$data<br>\n";
  echo "<br>\n";
}

?>

</body>

</html>



