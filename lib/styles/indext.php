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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);
Assets_Page::header (Locale_Translate::_("Styles"));
$view = new Assets_View (__FILE__);
$database_config_user = Database_Config_User::getInstance();

if (isset ($_GET['sheet'])) {
  $sheet = $_GET['sheet'];
  if ($sheet == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Would you like to switch to another stylesheet?"));
    $database_styles = Database_Styles::getInstance();
    $sheets = $database_styles->getSheets();
    foreach ($sheets as $sheet) {
      $parameter = "&sheet=$sheet";
      $dialog_list->add_row ($sheet, $parameter);
    }
    $dialog_list->run ();
  } else {
    $database_config_user->setStylesheet ($sheet);
  }
}

$stylesheet = $database_config_user->getStylesheet();
$view->view->stylesheet = Filter_Html::sanitize ($stylesheet);
$view->render ("indext.php");
Assets_Page::footer ();

?>
