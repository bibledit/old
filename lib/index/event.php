<?php
header ('Content-Type: text/event-stream');
header ('Cache-Control: no-cache');

$time = time ();
echo "data: $time \n\n";

?>
