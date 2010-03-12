<html>
<body>
<?php

require("constants.php");
$handler = opendir($messagesdir);
while ($file = readdir($handler)) {
  if ($file != '.' && $file != '..') {
    unlink ("$messagesdir/$file");
  }
}
closedir($handler);

?>
</body>
</html>
