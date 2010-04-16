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
$database_sessions = Database_Sessions::getInstance ();
$database_sessions->getCurrentSessionId ();
$desktop = $_GET ['desktop'];
if (isset ($desktop)) {
  $database_sessions->setValue ('desktop', $desktop);
} else {
  $desktop = $database_sessions->getValue ('desktop');
}
page_access_level (Desktop_Level::access ($desktop));
Assets_Page::header (gettext ("Desktop") . " $desktop");

$assets_navigator = Assets_Navigator::getInstance();
$assets_navigator->actions ();

if ($desktop == "editusfm") {
  $text_usfm = Text_Usfm::getInstance();
  $text_usfm->actions ();
}
if ($desktop == "edittext") {
  $edit_text = Text_Editor::getInstance();
  $edit_text->actions ();
}

$assets_navigator->display ();
if ($desktop == "editusfm") {
  $text_usfm->display ();
}
if ($desktop == "edittext") {
  $edit_text->display ();
}

Assets_Page::footer ();
?>
