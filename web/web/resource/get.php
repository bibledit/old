<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
page_access_level (CONSULTANT_LEVEL);


@$resource = $_GET['resource'];
@$book = $_GET['book'];
@$chapter = $_GET['chapter'];
@$verse = $_GET['verse'];


if (!$book || !$chapter || !$verse) {
  echo gettext ("No resource available for") . " " . Filter_Books::passageDisplay ($book, $chapter, $verse);
  die;
}


$database_bibles = Database_Bibles::getInstance ();
$database_resources = Database_Resources::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();


$stylesheet = $database_config_general->getExportStylesheet ();
$bibles = $database_bibles->getBibles ();
$externals = $database_resources->getNames ();
$usfms = $database_usfmresources->getResources ();
$resources = $database_config_user->getActiveResources ();


$resource = $resources [$resource];


$isBible = in_array ($resource, $bibles);
$isUsfm = in_array ($resource, $usfms);
if ($isBible || $isUsfm) {
  if ($isBible) $chapter_usfm = $database_bibles->getChapter ($resource, $book, $chapter);
  if ($isUsfm) $chapter_usfm = $database_usfmresources->getUsfm ($resource, $book, $chapter);
  $verse_usfm = Filter_Usfm::getVerseText ($chapter_usfm, $verse);
  $filter_text = new Filter_Text ("");
  $filter_text->html_text_standard = new Html_Text (gettext ("Bible"));
  $filter_text->addUsfmCode ($verse_usfm);
  $filter_text->run ($stylesheet);
  $html = $filter_text->html_text_standard->getHtml ();
} else if (in_array ($resource, $externals)) {
  // Use offline copy if it exists, else fetch it online.
  if ($database_offlineresources->exists ($resource, $book, $chapter, $verse)) {
    $html = $database_offlineresources->get ($resource, $book, $chapter, $verse);
  } else {
    $html = Resource_Logic::getExternal ($resource, $book, $chapter, $verse);
  }
} else {
  $html = gettext ("This resource is not available");
}


echo $html;


?>
