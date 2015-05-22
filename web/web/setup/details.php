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


error_reporting (E_ALL);


require_once ("../bootstrap/bootstrap.php");


$database_users  = Database_Users::getInstance ();


@$admin_username = $_POST ['admin_username'];
@$admin_password = $_POST ['admin_password'];
@$admin_email = $_POST ['admin_email'];
$error = array ();


if (isset ($admin_username)) {
  if (strlen ($admin_username) < 5) {
    unset ($admin_username);
    $error [] = "Choose a longer username.";
  }
}


if (isset ($admin_password)) {
  if (strlen ($admin_password) < 7) {
    unset ($admin_password);
    $error [] = "Choose a longer password.";
  }
}


if (isset ($admin_email)) {
  $validator = new Zend_Validate_EmailAddress ();
  if (!$validator->isValid ($admin_email)) {
    unset ($admin_email);
    $error [] = "Choose a valid email address.";
  }
}


if (isset ($admin_username) && isset ($admin_password) && isset ($admin_email)) {
  $database_users->removeUser ($admin_username);
  $database_users->addNewUser ($admin_username, $admin_password, Filter_Roles::ADMIN_LEVEL, $admin_email);
}



$admins = $database_users->getAdministrators ();
if (count ($admins) != 0) {
  include ("../filter/url.php");
  Filter_Url::redirect ("step.php?referer=details");
  die;
}


$error = implode (" ", $error);


?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Bibledit-Web Installation</title>
<link rel="stylesheet" href="stylesheet.css" type="text/css" />
</head>
<body>
<h1><img src="../assets/bibledit.png"> <a href="http://bibledit.org/">Bibledit-Web</a></h1>
<p>Please provide the following information. You can change these settings later.</p>
<p><?php echo $error ?></p>
<form id="setup" method="post" action="details.php">
  <table class="form-table">
    <tr>
      <th scope="row"><label for="user_login">Administrator's username</label></th>
      <td>
        <input name="admin_username" type="text" id="admin_username" size="25" value="" />
        <p>The username should be at least five characters long, and can have only alphanumeric characters, underscores, and hyphens.</p>
      </td>
    </tr>
    <tr>
      <th scope="row"><label for="admin_password">Administrator's password</label></th>
      <td>
        <input name="admin_password" type="password" id="admin_password" size="25" value="" />
        <p>Hint: The password should be at least seven characters long. To make it stronger, use upper and lower case letters, numbers and symbols like ! " ? $ % ^ &amp; ).</p>      </td>
    </tr>
    <tr>
      <th scope="row"><label for="admin_email">Your E-mail</label></th>
      <td>
        <input name="admin_email" type="email" id="admin_email" size="25" value="" />
        <p>Double-check your email address before continuing.</p>
      </td>
    </tr>
  </table>
  <p class="step"><input type="submit" name="Submit" value="Store settings" class="button button-large" /></p>
</form>
<script type="text/javascript">var t = document.getElementById ('admin_username').focus (); if (t) { t.focus (); }</script>
</body>
</html>
