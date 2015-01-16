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


page_access_level (Filter_Roles::translator ());


$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_books = Database_Books::getInstance ();


$sourceBible = request->database_config_user()->getSourceXrefBible ();
$targetBible = request->database_config_user()->getTargetXrefBible ();


// Save book / abbreviation pair.
if (isset (request->post ['save'])) {
  $fullname = request->post ['fullname'];
  $abbreviation = request->post ['abbreviation'];
  $abbreviations = Database_Config_Bible::getBookAbbreviations ($targetBible);
  $abbreviations = filter_abbreviations_display ($abbreviations);
  $abbreviations += "\n$fullname = $abbreviation";
  Database_Config_Bible::setBookAbbreviations ($targetBible, $abbreviations);
}


$sourceAbbreviations = Database_Config_Bible::getBookAbbreviations ($sourceBible);
$sourceAbbreviations = filter_abbreviations_read ($sourceAbbreviations);
$sourceAbbreviations = array_values ($sourceAbbreviations);


$targetAbbreviations = Database_Config_Bible::getBookAbbreviations ($targetBible);
$targetAbbreviations = filter_abbreviations_read ($targetAbbreviations);
$targetAbbreviations = array_values ($targetAbbreviations);


$unknown_abbreviations = filter_string_array_diff ($sourceAbbreviations, $targetAbbreviations);
$unknown_abbreviations = array_unique ($unknown_abbreviations);
for ($unknown_abbreviations as &$abbreviation) {
  $abbreviation = Database_Books::getEnglishFromId ($abbreviation);
}
$unknown_abbreviations = array_values ($unknown_abbreviations);


if (empty ($unknown_abbreviations)) {
  redirect_browser ("clear");
  die;
}


$header = new Assets_Header (gettext("Cross references"));
$header->run ();


$view = new Assets_View (__FILE__);


$view->view->remaining = count ($unknown_abbreviations) - 1;


$view->view->bookname = $unknown_abbreviations [0];


$view->render ("translate");


Assets_Page::footer ();


?>
