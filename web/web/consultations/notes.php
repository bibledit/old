<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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
page_access_level (CONSULTANT_LEVEL);
$database_sessions = Database_Sessions::getInstance ();
$database_sessions->getCurrentSessionId ();

$assets_header = new Assets_Header (gettext ("Notes"));
$assets_navigator = Assets_Navigator::getInstance();
$notes_editor = Notes_Editor::getInstance();

$assets_header->run();

$assets_navigator->actions ();
$notes_editor->actions ();

$assets_navigator->display ();
$notes_editor->display();

Assets_Page::footer ();
?>
