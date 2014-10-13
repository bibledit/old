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
page_access_level (Filter_Roles::GUEST_LEVEL);
Assets_Page::header (Locale_Translate::_("Signup"));


$view = new Assets_View (__FILE__);


/**
* Set of security questions.
*/
$questions[] = Locale_Translate::_("To which city was Paul travelling when a light from heaven shone round about him?");
$answers[]   = Locale_Translate::_("Damascus");
$passages[]  = Locale_Translate::_('And while he travelled, he came near Damascus; and suddenly a light from heaven shone round about him.');

$questions[] = Locale_Translate::_("What is the name of the brother of Aaron the high priest?");
$answers[]   = Locale_Translate::_("Moses");
$passages[]  = Locale_Translate::_('And the anger of Jehova was kindled against Moses, and he said: Is not Aaron the Levite your brother?');

$questions[] = Locale_Translate::_("What is the name of the city where Jesus was born?");
$answers[]   = Locale_Translate::_("Bethlehem");
$passages[]  = Locale_Translate::_('When Jesus was born in Bethlehem of Judaea in the days of Herod the king, behold, wise men from the east came to Jerusalem.');

$questions[] = Locale_Translate::_("What is the name of the island where John was sent to?");
$answers[]   = Locale_Translate::_("Patmos");
$passages[]  = Locale_Translate::_('I, John, your brother and companion in the persecution, and in the kingdom and endurance of Jesus Christ, was in the island which is called Patmos, because of the word of God, and because of the testimony of Jesus Christ.');

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
    $view->view->username_invalid_message = Locale_Translate::_("Username should be at least two characters long");
  }
  if (strlen ($pass) < 2) {
    $form_is_valid = false;
    $view->view->password_invalid_message = Locale_Translate::_("Password should be at least two characters long");
  }
  $validator = new Zend_Validate_EmailAddress ();
  if (!$validator->isValid ($mail)) {
    $form_is_valid = false;
    $view->view->email_invalid_message = Locale_Translate::_("The email address is not valid");
  }
  if ($answer != $standard) {
    $form_is_valid = false;
    $view->view->answer_invalid_message = Locale_Translate::_("The answer to the question is not correct");
  }
  $database_users = Database_Users::getInstance ();
  if ($form_is_valid) {
    if ($database_users->usernameExists ($user)) {
      $view->view->error_message = Locale_Translate::_("The username that you have chosen has already been taken. Please choose another one.");
      $form_is_valid = false;
    }
  }
  if ($form_is_valid) {
    if ($database_users->emailExists ($mail)) {
      $view->view->error_message = Locale_Translate::_("The email address that you have chosen has already been taken. Please choose another one.");
      $form_is_valid = false;
    }
  }
  if ($form_is_valid) {
    $confirm_worker = Confirm_Worker::getInstance ();
    $initial_subject = Locale_Translate::_("Signup verification");
    $initial_body = Locale_Translate::_("Somebody requested to open an account with this email address.");
    $query = $database_users->addNewUserQuery ($user, $pass, Filter_Roles::MEMBER_LEVEL, $mail);
    $subsequent_subject = Locale_Translate::_("Account opened");
    $subsequent_body = Locale_Translate::_("Welcome! Your account is now active.");
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
