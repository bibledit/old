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

$view = new Assets_View (__FILE__);

$object = $_GET ['object'];
$view->view->object = $object;

$database_config_bible = Database_Config_Bible::getInstance();
$url = $database_config_bible->getRemoteRepositoryUrl ($object);
$view->view->url = $url;

$ready = false;
$database_shell = Database_Shell::getInstance ();
$output = "";
$contents = array ();
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
$view->view->contents = $contents;

Assets_Page::header (Locale_Translate::_("Collaboration"));

// Display the page(s).
$view->render ("collaboration_take_yourself1.php");
if ($ready) $view->render ("collaboration_take_yourself2.php");

Assets_Page::footer ();

?>
