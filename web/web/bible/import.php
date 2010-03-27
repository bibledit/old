<?php

// Todo work on php and tpl file.

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_bibles = Database_Bibles::getInstance();
$database_books = Database_Books::getInstance();

// The name of the Bible.
$bible = $_GET['bible'];
$smarty->assign ("bible", Filter_Html::sanitize ($bible));

// USFM data submission.
if (isset($_POST['submit'])) {
  $data = $_POST['data'];
  $data = trim ($data);
  if ($data != "") {
    if (Validate_Utf8::valid ($data)) {
      $data = Filter_Usfm::oneString ($data);
      if (strpos ($data, "\\id ") === 0) {
        $markers_and_text = Filter_Usfm::getMarkersAndText ($data);
        foreach ($markers_and_text as $marker_or_text) {
          echo "$marker_or_text<br>"; // Todo
        }
      } else {
        $error_message = gettext ("The text should start with the \id markup, for example \"\id GEN\" for the book of Genesis.");
      }
    } else {
      $error_message = gettext ("Please supply valid Unicode UTF-8 text.");
    }
  } else {
    $success_message = gettext ("Nothing was imported.");
  }
}




$smarty->assign ("success_message", $success_message);
$smarty->assign ("error_message", $error_message);
$smarty->display ("import.tpl");

?>
