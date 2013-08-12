<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
page_access_level (GUEST_LEVEL);
// Postpone displaying header due to an expected changed logged-in state during this script.
$view = new Assets_View (__FILE__);

// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user = $_POST['user'];
  $pass = $_POST['pass'];
  if (strlen ($user) < 2) {
    $form_is_valid = false;
    $view->view->username_or_email_invalid_message = gettext ("Username should be at least two characters long");
  }
  if (strlen ($pass) < 4) {
    $form_is_valid = false;
    $view->view->password_invalid_message = gettext ("Password should be at least four characters long");
  }
  if ($form_is_valid) {
    $session_logic = Session_Logic::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    if ($session_logic->attemptLogin ($user, $pass)) {
      // Log the login.
      $database_logs->log ($session_logic->currentUser () . " logged in");
      // Store web site's base URL.
      $database_config_general = Database_Config_General::getInstance ();
      @$siteUrl = dirname (dirname ($_SERVER["HTTP_REFERER"]));
      $database_config_general->setSiteURL ($siteUrl);
    } else {
      $view->view->error_message = gettext ("Username or email address or password are not correct");
      $session_logic->logout();
      // Log the login failure.
      $database_logs->log ("Failed login attempt for user $user with password $pass");
    }
  }
}

// Delayed header display.
$header = new Assets_Header (gettext ("Login"));
$header->setBodyOnload ('document.form.user.focus();');
$header->run ();

$session_logic = Session_Logic::getInstance ();
@$query = $_SERVER['QUERY_STRING'];
if ($session_logic->loggedIn ()) {
  $query = substr ($query, 8);
  if ($query) {
    // After login, the user is forwarded to the originally requested URL, if any.
    header ("Location: " . $query);
    die;
  }
  $mail = Database_Mail::getInstance ();
  $mailcount = $mail->getMailCount ();
  $view->view->mailcount = $mailcount;
  $view->render ("loggedin.php");
} else {
  $view->view->query = $query;
  $view->view->logging_in = true;
  $view->render ("login.php");
}

Assets_Page::footer ();

?>
