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


$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance();


$stylesheet = $database_config_user->getStylesheet ();
$bible = $database_config_user->getTargetXrefBible ();


$book = $_GET ['book'];
$chapter = $_GET ['chapter'];
$verse = $_GET ['verse'];


$usfm = $database_bibles->getChapter ($bible, $book, $chapter);
$usfm = Filter_Usfm::getVerseText ($usfm, $verse);


$editor_import = Editor_Import::getInstance ();
$editor_import->load ($usfm);
$editor_import->stylesheet ($stylesheet);
$editor_import->run ();
$html = $editor_import->get ();


echo $html;


?>
