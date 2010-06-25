<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

Assets_Page::header (gettext ("Send/Receive"));

$smarty = new Smarty_Bibledit (__FILE__);

$ready = false;
$database_shell = Database_Shell::getInstance ();
$output = "";
switch ($database_shell->logic ("sendreceive", 0, $output)) {
  case 1: 
    $workingdirectory = dirname (__FILE__);
    shell_exec ("cd $workingdirectory; php sendreceive-cli.php > $output 2>&1 &");
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
$smarty->display("sendreceive1.tpl");
if ($ready) $smarty->display("sendreceive2.tpl");

Assets_Page::footer ();

?>
