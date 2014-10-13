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
?>
<h1><?php echo Locale_Translate::_("Signup") ?></h1>

<form action="signup.php" method="post">

  <table>

  <p class="error"><?php echo $this->error_message ?></p>

  <tr>
  <td align="right"><?php echo Locale_Translate::_("Which username would you like to use?") ?></td>
  <td><input type="text" name="user" maxlength="30" /></td>
  <td class="error"><?php echo $this->username_invalid_message ?></td>
  </tr>

  <tr>
  <td align="right"><?php echo Locale_Translate::_("Which password?") ?></td>
  <td><input type="password" name="pass" maxlength="30" /></td>
  <td class="error"><?php echo $this->password_invalid_message ?></td>
  </tr>

  <tr>
  <td align="right"><?php echo Locale_Translate::_("Your email address?") ?></td>
  <td><input type="text" name="mail" maxlength="256" /></td>
  <td class="error"><?php echo $this->email_invalid_message ?></td>
  </tr>

  <tr>
  <td align="right"><?php echo Locale_Translate::_("Security question:") ?> <?php echo $this->question ?></td>
  <td><input type="text" name="answer" maxlength="40" /></td>
  <td class="error"><?php echo $this->answer_invalid_message ?></td>
  </tr>

  <tr>
  <td align="right"></td>
  <td><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Signup") ?> /></td>
  </tr>

  </table>

  <input type="hidden" name="standard" maxlength="40" value=<?php echo $this->standard ?> />

  <p><?php echo Locale_Translate::_("Hint to answer the security question:") ?> <?php echo $this->passage ?></p>

</form>
