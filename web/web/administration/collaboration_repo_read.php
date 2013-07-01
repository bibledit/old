<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Collaboration"));
$view = new Assets_View (__FILE__);

$object = $_GET ['object'];
$view->view->object = $object;

$database_config_user = Database_Config_User::getInstance();
if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$view->view->url = $url;

// Create the git repository directory now since this is the most convenient moment to do it.
$directory = Filter_Git::git_directory ($object);
Filter_Rmdir::rmdir ($directory);
mkdir ($directory, 0777, true);

$command = "git ls-remote $url 2>&1";
$view->view->command = $command;
ob_start ();
system ($command, $exit_code);
$read_access_result = ob_get_contents ();
ob_end_clean();
$view->view->read_access_result = nl2br ($read_access_result);

if ($exit_code == 0) {
  $success_message = gettext ("Read access to the repository is successful.");
} else {
  $error_message = gettext ("Read access failed. Please retry it, possibly with another URL.");
}
@$view->view->success_message = $success_message;
@$view->view->error_message = $error_message;

  $view->render ("collaboration_repo_read.php");

Assets_Page::footer ();

?>
