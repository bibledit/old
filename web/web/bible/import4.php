<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Import"));
set_time_limit (0);

$smarty = new Smarty_Bibledit (__FILE__);

$bible = $_GET['bible'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));

$location = $_GET ['location'];
if (file_exists ($location)) {
  $workingdirectory = dirname (__FILE__);
  $bible = escapeshellarg ($bible);
  $location = escapeshellarg ($location);
  $command = "php importcli.php $location $bible > /dev/null 2>&1 &";
  shell_exec ($command);
  $smarty->display ("import4.tpl");
} else {
  Assets_Page::error (gettext ("Could not find the folder with the consultation notes"));
}
Assets_Page::footer ();

?>
