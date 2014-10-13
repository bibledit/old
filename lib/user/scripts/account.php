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
<h1><?php echo Locale_Translate::_("Account") ?> <?php echo $this->username ?></h1>

<form action="account.php" name="form" method="post">

<table>

<p class="error"><?php echo $this->error_message ?></p>
<?php foreach ($this->success_messages as $success_message) { ?>
  <p class="success"><?php echo $success_message ?></p>
<?php } ?>

<?php if ($this->actions_taken == false) { ?>

<p><?php echo Locale_Translate::_("You can change your password or your email address. Your current email address is") ?> <?php echo $this->email ?>.</p>

<tr>
<td align="right"><?php echo Locale_Translate::_("Current password") ?></td>
<td><input type="text" name="currentpassword" maxlength="30" /></td>
<td class="error"><?php echo $this->current_password_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo Locale_Translate::_("New password") ?></td>
<td><input type="text" name="newpassword" maxlength="30" /></td>
<td class="error"><?php echo $this->new_password_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo Locale_Translate::_("Again new password") ?></td>
<td><input type="text" name="newpassword2" maxlength="30" /></td>
<td class="error"><?php echo $this->new_password2_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo Locale_Translate::_("New email address") ?></td>
<td><input type="text" name="newemail" maxlength="256" /></td>
<td class="error"><?php echo $this->new_email_invalid_message ?></td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Submit") ?> /></td>
</tr>

<?php } ?>

</table>

</form>
