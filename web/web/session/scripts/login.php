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
?>
<h1><?php echo gettext ("Login") ?></h1>

<form action="login.php?<?php echo $this->query ?>" name="form" method="post">

<table>

<p class="error"><?php echo $this->error_message ?></p>

<tr>
<td align="right"><?php echo gettext ("Username or email address") ?></td>
<td><input type="text" name="user" maxlength="30" class="focus" /></td>
<td class="error"><?php echo $this->username_or_email_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo gettext ("Password") ?></td>
<td><input type="password" name="pass" maxlength="30" /></td>
<td class="error"><?php echo $this->password_invalid_message ?></td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value=<?php echo gettext ("Login") ?> /></td>
</tr>

</table>

<p><a href="password.php"><?php echo gettext ("Forgot password?") ?></a></p>

<p><?php echo gettext ("Not registered?") ?> <a href="signup.php"><?php echo gettext ("Sign up!") ?></a></p>
</form>
