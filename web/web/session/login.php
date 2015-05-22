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


$database_logs = Database_Logs::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$session_logic = Session_Logic::getInstance ();


/*
Postpone displaying the header for two reasons:
1. The logged-in state is likely to change during this script.
   Therefore the header should wait till the new state is known.
2. The script may forward the user to another page.
   Therefore no output should be sent so the forward headers work.


This script can have several functions:

1. Display login form.
The user is not logged in. 
The standard form is displayed.

2. Incorrect credentials entered.
The user did not enter correct credentials.
The login form is displayed, with an error description.

3. Forward to URL.
The script is called with a query for where to forward the user to.
*/


$view = new Assets_View (__FILE__);


// Form submission handler.
if (isset($_POST['submit'])) {
  $form_is_valid = true;
  $user = $_POST['user'];
  $pass = $_POST['pass'];
  if (strlen ($user) < 2) {
    $form_is_valid = false;
    $view->view->username_or_email_invalid_message = Locale_Translate::_("Username should be at least two characters long");
  }
  if (strlen ($pass) < 4) {
    $form_is_valid = false;
    $view->view->password_invalid_message = Locale_Translate::_("Password should be at least four characters long");
  }
  if ($form_is_valid) {
    if ($session_logic->attemptLogin ($user, $pass)) {
      // Log the login.
      $database_logs->log ($session_logic->currentUser () . " logged in");
      // Store web site's base URL.
      @$siteUrl = dirname (dirname ($_SERVER["HTTP_REFERER"]));
      $database_config_general->setSiteURL ($siteUrl);
    } else {
      $view->view->error_message = Locale_Translate::_("Username or email address or password are not correct");
      $session_logic->logout();
      // Log the login failure for the Administrator(s), so that others cannot reverse engineer a user's password based on the failure information.
      $database_logs->log ("Failed login attempt for user $user with password $pass", Filter_Roles::ADMIN_LEVEL);
    }
  }
}


@$query = $_SERVER['QUERY_STRING'];
if ($session_logic->loggedIn ()) {
  $query = substr ($query, 8);
  if ($query) {
    // After login, the user is forwarded to the originally requested URL, if any.
    // Cannot use Filter_Url::redirect here, so the Location: header is not standards compliant now.
    header ("Location: " . $query);
    die;
  }
  display_header ();
  $view->render ("loggedin.php");
} else {
  $view->view->query = $query;
  $view->view->logging_in = true;
  display_header ();
  $view->render ("login.php");
}

Assets_Page::footer ();


function display_header ()
{
  // Delayed header display.
  $header = new Assets_Header (Locale_Translate::_("Login"));
  $header->setBodyOnload ('document.form.user.focus();');
  $header->run ();
}


?>
