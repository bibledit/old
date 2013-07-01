<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Collaboration"));
$view = new Assets_View (__FILE__);

$object = $_GET ['object'];
$view->view->object = $object;

$directory = Filter_Git::git_directory ($object);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$view->view->url = $url;

$ready = false;
$database_shell = Database_Shell::getInstance ();
$output = "";
$contents = array ();
switch ($database_shell->logic ("collaboration_repo_write", 0, $output)) {
  case 1: 
    $workingdirectory = dirname (__FILE__);
    $object = escapeshellarg ($object);
    $directory = escapeshellarg ($directory);
    shell_exec ("cd $workingdirectory; php collaboration_repo_write-cli.php $object $directory > $output 2>&1 &");
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

$view->render ("collaboration_repo_write1.php");
if ($ready) $view->render ("collaboration_repo_write2.php");

Assets_Page::footer ();

?>
