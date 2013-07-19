<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (CONSULTANT_LEVEL);


@$pollpassage = $_GET['pollpassage'];
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


@$book = $_GET['book'];
@$chapter = $_GET['chapter'];
@$verse = $_GET['verse'];


if (!$book || !$chapter || !$verse) {
  echo gettext ("No resources available for") . " " . Filter_Books::passageDisplay ($book, $chapter, $verse);
  die;
}


$view = new Assets_View (__FILE__);


$fragments = array ();


$database_bibles = Database_Bibles::getInstance ();
$database_resources = Database_Resources::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();


$stylesheet = $database_config_general->getExportStylesheet ();
$bibles = $database_bibles->getBibles ();
$externals = $database_resources->getNames ();
$actives = $database_config_user->getActiveResources ();

foreach ($actives as $active) {
  if (in_array ($active, $bibles)) {
    $chapter_usfm = $database_bibles->getChapter ($active, $book, $chapter);
    $verse_usfm = Filter_Usfm::getVerseText ($chapter_usfm, $verse);
    $filter_text = new Filter_Text ("");
    $filter_text->html_text_standard = new Html_Text (gettext ("Bible"));
    $filter_text->addUsfmCode ($verse_usfm);
    $filter_text->run ($stylesheet);
    $html = $filter_text->html_text_standard->getHtml ();
  } else if (in_array ($active, $externals)) {
    $html = Resource_Logic::getExternal ($active, $book, $chapter, $verse);
  } else {
    $html = "Unavailable";
  }
  $fragments [] = $html;
}


$view->view->names = $actives;
$view->view->fragments = $fragments;
$view->render ("get.php");


?>
