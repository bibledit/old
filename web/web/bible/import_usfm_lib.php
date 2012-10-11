<?php


require_once ("../bootstrap/bootstrap.php");


function import_usfm ($bible, $data, &$success_message, &$error_message) {
  $database_config_user = Database_Config_User::getInstance();
  $stylesheet = $database_config_user->getStylesheet();
  $book_chapter_text = Filter_Usfm::import ($data, $stylesheet);
  foreach ($book_chapter_text as $data) {
    $book_number = $data[0];
    $chapter_number = $data[1];
    $chapter_data = $data[2];
    if ($book_number > 0) {
      $database_bibles = Database_Bibles::getInstance();
      $database_bibles->storeChapter ($bible, $book_number, $chapter_number, $chapter_data);
      $database_snapshots = Database_Snapshots::getInstance();
      $database_snapshots->snapChapter ($bible, $book_number, $chapter_number, $chapter_data, 1);
      $database_books = Database_Books::getInstance ();
      $book_name = $database_books->getUsfmFromId ($book_number);
      $success_message .= " $book_name $chapter_number"; 
    } else {
      $error_message .= " $chapter_data"; 
    }
  }
  if ($error_message != "") {
    $error_message = gettext ("Could not import this data:") . $error_message;
  }
  if ($success_message != "") {
    $success_message = gettext ("The following was imported:") . $success_message;
  }
}


?>
