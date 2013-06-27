<?php
/**
* User signup.
* @package bibledit
*/
    

require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);
Assets_Page::header (gettext ("Signup"));


$view = new Assets_View (__FILE__);


/**
* Set of security questions.
*/
$questions[] = gettext ("To which city was Paul travelling when a light from heaven shone round about him?");
$answers[]   = gettext ("Damascus");
$passages[]  = gettext ('And while he travelled, he came near Damascus; and suddenly a light from heaven shone round about him.');

$questions[] = gettext ("What is the name of the brother of Aaron the high priest?");
$answers[]   = gettext ("Moses");
$passages[]  = gettext ('And the anger of Jehova was kindled against Moses, and he said: Is not Aaron the Levite your brother?');

$questions[] = gettext ("What is the name of the city where Jesus was born?");
$answers[]   = gettext ("Bethlehem");
$passages[]  = gettext ('When Jesus was born in Bethlehem of Judaea in the days of Herod the king, behold, wise men from the east came to Jerusalem.');

$questions[] = gettext ("What is the name of the island where John was sent to?");
$answers[]   = gettext ("Patmos");
$passages[]  = gettext ('I, John, your brother and companion in the persecution, and in the kingdom and endurance of Jesus Christ, was in the island which is called Patmos, because of the word of God, and because of the testimony of Jesus Christ.');

$question_number = rand (0, 3);
$view->view->question = $questions[$question_number];
$view->view->passage = $passages[$question_number];
// The form has a hidden text entry. This text entry stores the right answer to the questions.
// When the form is submitted, this right answer is submitted too, and we can check whether 
// the user gave the right answer.
$view->view->standard = $answers[$question_number];

// Form submission handler.
$signed_up = false;
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user     = $_POST['user'];
  $pass     = $_POST['pass'];
  $mail     = $_POST['mail'];
  $answer   = $_POST['answer'];
  $standard = $_POST['standard'];
  if (strlen ($user) < 2) {
    $form_is_valid = false;
    $view->view->username_invalid_message = gettext ("Username should be at least two characters long");
  }
  if (strlen ($pass) < 2) {
    $form_is_valid = false;
    $view->view->password_invalid_message = gettext ("Password should be at least two characters long");
  }
  $validator = new Zend_Validate_EmailAddress ();
  if (!$validator->isValid ($mail)) {
    $form_is_valid = false;
    $view->view->email_invalid_message = gettext ("The email address is not valid");
  }
  if ($answer != $standard) {
    $form_is_valid = false;
    $view->view->answer_invalid_message = gettext ("The answer to the question is not correct");
  }
  $database_users = Database_Users::getInstance ();
  if ($form_is_valid) {
    if ($database_users->usernameExists ($user)) {
      $view->view->error_message = gettext ("The username that you have chosen has already been taken. Please choose another one.");
      $form_is_valid = false;
    }
  }
  if ($form_is_valid) {
    if ($database_users->emailExists ($mail)) {
      $view->view->error_message = gettext ("The email address that you have chosen has already been taken. Please choose another one.");
      $form_is_valid = false;
    }
  }
  if ($form_is_valid) {
    $confirm_worker = Confirm_Worker::getInstance ();
    $initial_subject = gettext ("Signup verification");
    $initial_body = gettext ("Somebody requested to open an account with this email address.");
    include ("session/levels.php");
    $query = $database_users->addNewUserQuery ($user, $pass, MEMBER_LEVEL, $mail);
    $subsequent_subject = gettext ("Account opened");
    $subsequent_body = gettext ("Welcome! Your account is now active.");
    $confirm_worker->setup ($mail, $initial_subject, $initial_body, $query, $subsequent_subject, $subsequent_body);
    $signed_up = true;
  }
}


if ($signed_up)
  $view->render ("signedup.php");
else
  $view->render ("signup.php");

Assets_Page::footer ();


?>
