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
$bible = $database_config_user->getSourceXrefBible ();


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


// The source text has notes and cross references.
// The target eventually gets them too.
// The links like <a href="#note1" id="citation1" ...  are the same in both.
// That applies to <a href="#citation1" id="note1"> ... too.
// Therefore clicking a link in the source text may take the user to the target
// text, and vice versa.
// Fix the source text, because the source text can be modified, since it won't be saved.
for ($i = 1; $i < 1000; $i++) {
  $html = str_replace ('note' . $i . '"', 'note' . ($i + 1000) . '"', $html);
  $html = str_replace ('citation' . $i . '"', 'citation' . ($i + 1000) . '"', $html);
}


echo $html;


?>
