<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::admin ());


Assets_Page::header (gettext("Collaboration"));
$view = new Assets_View (__FILE__);


@$object = request->query ['object'];
@$select = request->query['select'];
if (isset ($select)) {
  if ($select == "") {
    $dialog_list = new Dialog_List (array ("object"), gettext("Which Bible are you going to use?"), "", "");
    $database_bibles = Database_Bibles::getInstance();
    $bibles = request->database_bibles()->getBibles();
    for ($bibles as $value) {
      $dialog_list->add_row ($value, "&select=$value");
    }
    $dialog_list->run ();
    die;
  } else {
    $object = $select;
  }
}
$view->view->object = $object;


$database_config_bible = Database_Config_Bible::getInstance();
$url = Database_Config_Bible::getRemoteRepositoryUrl ($object);
if (isset (request->query ['disable'])) {
  $url = "";
  Database_Config_Bible::setRemoteRepositoryUrl ($object, $url);
  $repository = filter_git_git_directory ($object);
  filter_url_rmdir ($repository);
}
$url = Database_Config_Bible::getRemoteRepositoryUrl ($object);
$view->view->url = $url;


$git = new Filter_Which ("git");
$git = $git->available;
$view->view->git_available = $git;


$view->render ("collaboration");
Assets_Page::footer ();


?>
