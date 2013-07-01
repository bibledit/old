<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Collaboration"));
$view = new Assets_View (__FILE__);

$which_git = new Filter_Which ("git");
$view->view->git = $which_git->available;

$object = $_GET ['object'];
$view->view->object = $object;

$database_config_user = Database_Config_User::getInstance();
if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$view->view->url = $url;

ob_start ();
$username = system ("whoami");
ob_end_clean();
$view->view->username = $username;

$view->render ("collaboration_flash_drive_setup.php");
Assets_Page::footer ();
?>
