<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Import BibleWorks"));


$view = new Assets_View (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();


$success_message = "";
$error_message = "";


// The name of the Bible.
$bible = $_GET['bible'];
$view->view->bible = Filter_Html::sanitize ($bible);


// Data submission.
if (isset($_POST['submit'])) {
  // Whether to keep the grammatical tags.
  $tags = isset($_POST['tags']);
  // The BibleWorks text.
  $data = $_POST['data'];
  $data = trim ($data);
  if ($data != "") {
    if (Validate_Utf8::valid ($data)) {
      // Convert the BibleWorks text to USFM.
      $usfm = Filter_Bibleworks::import ($data, $tags);
      // Import the USFM.
      include_once ("import_usfm_lib.php");
      import_usfm ($bible, $usfm, $success_message, $error_message);
    } else {
      $error_message = gettext ("Please supply valid Unicode UTF-8 text.");
    }
  } else {
    $success_message = gettext ("Nothing was imported.");
  }
}


@$view->view->success_message = $success_message;
@$view->view->error_message = $error_message;
$view->render ("import_bibleworks.php");
Assets_Page::footer ();


?>
