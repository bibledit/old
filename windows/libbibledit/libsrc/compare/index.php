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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


@$bible = $_GET ["bible"];


@$compare = $_GET ["compare"];


if (isset ($compare)) {
  $database_jobs = Database_Jobs::getInstance ();
  $jobId = $database_jobs->getNewId ();
  $database_jobs->setLevel ($jobId, Filter_Roles::CONSULTANT_LEVEL);
  Tasks_Logic::queue (Tasks_Logic::PHP, array (__DIR__ . "/comparecli.php", $bible, $compare, "$jobId"));
  Filter_Url::redirect ("../jobs/index.php?id=$jobId");
  die;
}


$header = new Assets_Header (Locale_Translate::_("Compare"));
$header->run ();


// Names of the Bibles and the USFM Resources.
$names = array ();

$database_bibles = Database_Bibles::getInstance ();
$bibles = $database_bibles->getBibles ();
$names = array_merge ($names, $bibles);

$database_usfmresources = Database_UsfmResources::getInstance ();
$usfm_resources = $database_usfmresources->getResources ();
$names = array_merge ($names, $usfm_resources);

$names = array_diff ($names, array ($bible));


$view = new Assets_View (__FILE__);
$view->view->bible = $bible;
$view->view->names = $names;
$view->render ("index.php");


Assets_Page::footer ();


?>
