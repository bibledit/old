<?php
/**
* User signup.
* @package bibledit
*/
    

require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);


$smarty = new Smarty_Bibledit (__FILE__);


/**
* Questions the user who signs up should be able to anwer.
*/
$questions[] = gettext ("To which city was Paul travelling when a light from heaven shone round about him?");
$answers[]   = gettext ("Damascus");
$passages[]  = gettext ('And while he travelled, he came near Damascus; and suddenly a light from heaven shone round about him.');

$questions[] = gettext ("What is the name of the brother of Aaron the high priest?");
$answers[]   = gettext ("Moses");
$passages[]  = gettext ('And the anger of Jehova was kindled against Moses, and he said: Is not Aaron the Levite your brother?');

$questions[] = gettext ("What is the name of the city where Jesus was born?");
$answers[]   = gettext ("Bethlehem");
$passages[]  = gettext ('Now when Jesus was born in Bethlehem of Judaea in the days of Herod the king, behold, wise men from the east came to Jerusalem.');

$questions[] = gettext ("What is the name of the island where John was sent to?");
$answers[]   = gettext ("Patmos");
$passages[]  = gettext ('I, John, your brother and companion in the persecution, and in the kingdom and endurance of Jesus Christ, was in the island which is called Patmos, because of the word of God, and because of the testimony of Jesus Christ.');

$question_number = rand (0, 3);
$smarty->assign ('question', $questions[$question_number]);
$smarty->assign ('passage', $passages[$question_number]);
// The form has a hidden text entry. This text entry stores the right answer to the questions.
// When the form is submitted, this right answer is submitted too, and we can check whether 
// the user gave the right answer.
$smarty->assign ('standard', $answers[$question_number]);

// Form submission handler.
$signed_up = false;
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user     = $_POST['user'];
  $pass     = $_POST['pass'];
  $mail     = $_POST['mail'];
  $answer   = $_POST['answer'];
  $standard = $_POST['standard'];
  if (strlen ($user) < 4) {
    $form_is_valid = false;
    $smarty->assign ('username_invalid_message', gettext ("Username should be at least four characters long"));
  }
  if (strlen ($pass) < 4) {
    $form_is_valid = false;
    $smarty->assign ('password_invalid_message', gettext ("Password should be at least four characters long"));
  }
  if (!Validate_Email::valid ($mail)) {
    $form_is_valid = false;
    $smarty->assign ('email_invalid_message', gettext ("The email address is not valid"));
  }
  if ($answer != $standard) {
    $form_is_valid = false;
    $smarty->assign ('answer_invalid_message', gettext ("The answer to the question is not correct"));
  }
  if ($form_is_valid) {
    $database = Database_Users::getInstance ();
    if ($database->usernameExists ($user)) {
      $smarty->assign ('error_message', gettext ("The username that you have chosen has already been taken. Please choose another one."));
    } else {
      include ("session/levels.php");
      $database->addNewUser($user, $pass, MEMBER_LEVEL, $mail);
      $signed_up = true;
    }
  }
}


if ($signed_up)
  $smarty->display("signedup.tpl");
else
  $smarty->display("signup.tpl");



?>
