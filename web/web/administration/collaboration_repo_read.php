<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::ADMIN_LEVEL);
Assets_Page::header (Locale_Translate::_("Collaboration"));
$view = new Assets_View (__FILE__);

$object = $_GET ['object'];
$view->view->object = $object;

$database_config_bible = Database_Config_Bible::getInstance();
if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_bible->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_bible->getRemoteRepositoryUrl ($object);
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
  $success_message = Locale_Translate::_("Read access to the repository is successful.");
} else {
  $error_message = Locale_Translate::_("Read access failed. Please retry it, possibly with another URL.");
}
@$view->view->success_message = $success_message;
@$view->view->error_message = $error_message;

  $view->render ("collaboration_repo_read.php");

Assets_Page::footer ();

?>
