<?php

require_once ("../bootstrap/bootstrap.php");

@$user = $_GET['user'];

@$channel = $_GET['channel'];

@$subject = $_GET['subject'];
if ($subject == "") die;

$message = $_GET['message'];
if ($message == "") die;

$database_ipc = Database_Ipc::getInstance();
$database_ipc->storeMessage ($user, $channel, $subject, $message);


?>
