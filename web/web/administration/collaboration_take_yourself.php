<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Collaboration"));

$smarty = new Smarty_Bibledit (__FILE__);

$object = $_GET ['object'];
$smarty->assign ("object", $object);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

$ready = false;
$database_shell = Database_Shell::getInstance ();
$output = "";
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
@$smarty->assign ("contents", $contents);

// Display the page(s).
$smarty->display("collaboration_take_yourself1.tpl");
if ($ready) $smarty->display("collaboration_take_yourself2.tpl");

Assets_Page::footer ();

?>