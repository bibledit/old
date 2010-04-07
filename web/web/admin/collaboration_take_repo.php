<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);

$bible = $_GET ['bible'];
$smarty->assign ("bible", $bible);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($bible);
$smarty->assign ("url", $url);

$directory = $_GET ['directory'];
$smarty->assign ("directory", $directory);

// Copy the data from the local cloned repository, and store it in Bibledit-Web's $bible,
// deleting the whole $bible that was there before.
Filter_Git::filedata2database ($directory, $bible);

// Store the git repository in the .git directory into Bibledit-Web's database,
// keeping it there for the next Send/Receive action.
Filter_Git::repository2database ($directory, $bible);

// Display the page.
$smarty->display("collaboration_take_repo.tpl");

?>
