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
<h1><?php echo Locale_Translate::_("New password") ?></h1>

<form action="password.php" name="form" method="post">

<table>

<p class="success"><?php echo $this->success_message ?></p>

<?php if ($this->success_message == "") { ?>

<p><?php echo Locale_Translate::_("If you lost the password, submit your username or email address, and a new password will be emailed to you.") ?></p>

<tr>
<td align="right"><?php echo Locale_Translate::_("Username or email address") ?></td>
<td><input type="text" name="user" maxlength="30" class="focus" /></td>
<td class="error"><?php echo $this->error_message ?></td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Submit") ?> /></td>
</tr>

<?php } ?>

</table>

</form>
