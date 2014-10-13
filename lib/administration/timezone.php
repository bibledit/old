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
Assets_Page::header (Locale_Translate::_("Timezone"));
$database_config_general = Database_Config_General::getInstance ();
@$timezone =  $_GET['timezone'];
if (isset ($timezone)) {
  if ($timezone == "") {
    $dialog_list = new Dialog_List2 (Locale_Translate::_("Would you like to change the default timezone for the site?"));
    $timezone_identifiers = Filter_Datetime::timezones();
    foreach ($timezone_identifiers as $timezone_identifier) {
      $dialog_list->add_row ($timezone_identifier, "&timezone=$timezone_identifier");
    }
    $dialog_list->run ();
  } else {
    $database_config_general->setTimezone ($timezone);
  }
}
$view = new Assets_View (__FILE__);
$timezone = $database_config_general->getTimezone();
$view->view->timezone = $timezone;
$view->render ("timezone.php");
Assets_Page::footer ();
?>
