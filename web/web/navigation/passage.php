<?php
require_once ("../bootstrap/bootstrap.php");
$ipc_focus = Ipc_Focus::getInstance();
$book = $ipc_focus->getBook ();
$chapter = $ipc_focus->getChapter ();
$verse = $ipc_focus->getVerse ();
$passage = array ("book" => $book, "chapter" => $chapter, "verse" => $verse);
$passage = json_encode ($passage);
echo $passage;
?>
