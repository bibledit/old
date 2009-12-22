<html>
<body>
<?php

$directory = "messages";
$handler = opendir($directory);
while ($file = readdir($handler)) {
  if ($file != '.' && $file != '..') {
    unlink ("$directory/$file");
  }
}
closedir($handler);

?>
</body>
</html>
