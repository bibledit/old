<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Collaboration"));

$view = new Assets_View (__FILE__);

$object = $_GET ['object'];
$view->view->object = $object;

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$view->view->url = $url;

$ready = false;
$database_shell = Database_Shell::getInstance ();
$output = "";
$contents = array ();
switch ($database_shell->logic ("collaboration_take_yourself", 0, $output)) {
  case 1: 
    $workingdirectory = dirname (__FILE__);
    $object = escapeshellarg ($object);
    shell_exec ("cd $workingdirectory; php collaboration_take_yourself-cli.php $object > $output 2>&1 &");
    break;
  case 0:
    $contents = file ($output, FILE_IGNORE_NEW_LINES);
    break;
  case -1:
    $contents = file ($output, FILE_IGNORE_NEW_LINES);
    $ready = true;
    break;
}
$view->view->contents = $contents;

// Display the page(s).
$view->render ("collaboration_take_yourself1.php");
if ($ready) $view->render ("collaboration_take_yourself2.php");

Assets_Page::footer ();

?>
