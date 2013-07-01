<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Collaboration"));
$view = new Assets_View (__FILE__);
$object = $_GET ['object'];
$view->view->object = $object;
$database_config_user = Database_Config_User::getInstance();
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$directory = Filter_Git::git_directory ($object);
$view->render ("collaboration_repo_data.php");
Assets_Page::footer ();
?>
