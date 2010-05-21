<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);

// Todo update for consultation notes.

$object = $_GET ['object'];
$smarty->assign ("object", $object);

$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

// Set up the secure keys just in case the repository happens to be secure.
$secure_key_directory = Filter_Git::git_config ($url);

$directory = $_GET ['directory'];
$smarty->assign ("directory", $directory);

// Take action based on whether it deals with a Bible or with the Consultation Notes.
if ($object == "consultationnotes") {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's consultation notes,
  // overwriting the whole Bible that was there before.
  Filter_Git::notesFiledata2database ($directory); // Todo try it.

} else {

  // Copy the data from the local cloned repository, and store it in Bibledit-Web's Bible given in $object,
  // overwriting the whole Bible that was there before.
  Filter_Git::bibleFiledata2database ($directory, $object);
  
}

// Store the git repository in the .git directory into Bibledit-Web's database,
// keeping it there for the next Send/Receive action.
Filter_Git::repository2database ($directory, $object);

// Display the page.
$smarty->display("collaboration_take_repo.tpl");

// For security reasons, remove the private ssh key.
Filter_Git::git_un_config ($secure_key_directory);

?>
