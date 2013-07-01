<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Collaboration"));

$view = new Assets_view (__FILE__);

$which_git = new Filter_Which ("git");
$view->view->git = $which_git->available;
$which_ssh = new Filter_Which ("ssh");
$view->view->ssh = $which_ssh->available;

$database_config_user = Database_Config_User::getInstance();
$database_config_general = Database_Config_General::getInstance();

$object = $_GET ['object'];
$view->view->object = $object;

if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$view->view->url = $url;

$view->render ("collaboration_secure_network_setup.php");
Assets_Page::footer ();
?>
