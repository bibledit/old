<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


@$pollpassage = $_GET['pollpassage']; // Todo
if (isset ($pollpassage)) {
  $ipc_focus = Ipc_Focus::getInstance();
  $book = $ipc_focus->getBook ();
  $chapter = $ipc_focus->getChapter ();
  $verse = $ipc_focus->getVerse ();
  $passage = array ("book" => $book, "chapter" => $chapter, "verse" => $verse);
  $passage = json_encode ($passage);
  echo $passage;
  die;
}


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
  $html = Resource_Logic::getExternal ($resource, $book, $chapter, $verse);
} else {
  $html = gettext ("This resource is not available");
}


echo $html;


?>
