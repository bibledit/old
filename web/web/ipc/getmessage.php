<?php

require_once ("../bootstrap/bootstrap.php");

$channel = $_GET['channel'];
$id = $_GET['id'];
if (!isset ($id)) $id = 0;

$database_ipc = Database_Ipc::getInstance();

@$values = "";
while (!is_array ($values)) {
  sleep (1);
  $values = $database_ipc->retrieveMessage ($id, $channel);
}

$id      = $values['id'];
$channel = $values['channel'];
$command = $values['command'];
$message = $values['message'];

if (is_int ($id)) {
  if ($channel != "") {
    $database_ipc->deleteMessage ($id);
  }
}

echo "$id\n";
echo "$command\n";
echo "$message\n";


?>
